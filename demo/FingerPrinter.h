#ifndef FINGERPRINTER_H_
#define FINGERPRINTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <stdbool.h>
#include <cstdio>
#include <unistd.h>

#include <math.h>
#include <stdarg.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#include <iostream>

#include <fcntl.h>
#include <poll.h>
#include <string.h>

#include <mutex>

#ifndef NDEBUG
#define DEBUG
#endif


#define MAX_BUF 512
#define SYSFS_GPIO_DIR "/sys/class/gpio"


using namespace std;

#define key_pin 29
#define SWITCH 7

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct AS608_Module_Info
{
  uint status;      // Status registers 0
  uint model;       // sencor type 0-15
  uint capacity;    // fingerprint capacity，300
  uint secure_level;    // safety level 1/2/3/4/5，default =3 
  uint packet_size;     // data package size 32/64/128/256 bytes，default = 128
  uint baud_rate;       // baudrate val 
  uint chip_addr;                      
  uint password;        
  char product_sn[12];        // product module number
  char software_version[12];  // software edition number
  char manufacture[12];
  char sensor_name[12];

  uint detect_pin;      // AS608 WAK pins with raspi gpios
  uint has_password;  
} AS_608;

typedef struct _Config {
  unsigned int address;
  unsigned int password;
  int has_password;
  int baudrate;
  int detect_pin;
  char serial[16];
} Config;
/****************************************************************************************************/
/****************************  Call Back INTERFACE declairation   **********************************/
/***************************************************************************************************/

/**
 * Callback for new samples which needs to be implemented by the main program.
 * The function hasSample needs to be overloaded in the main program.
 **/
typedef bool (*cb_getimage_ptr)(void*);

typedef bool (*cb_genchar_ptr)(void*, uchar);

typedef bool (*cb_exit_ptr)(void*);

typedef bool (*cb_search_ptr)(void*, uchar, int, int, int*, int*);


bool Get_getimage_CallBack(void* lpvoid, cb_getimage_ptr callback_param);

bool Get_genchar_CallBack(void* lpvoid, cb_genchar_ptr callback_param, uchar a);

bool Get_exit_CallBack(void* lpvoid, cb_exit_ptr callback_param);

bool Get_search_CallBack(void* lpvoid, cb_search_ptr callback_param, uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);


class FingerPrint_CallBack {
	public:
		FingerPrint_CallBack();
		~FingerPrint_CallBack();
		virtual void cb_func(int a) =0;
		virtual void cb_func2(int a) =0;
		virtual void cb_search() =0;
		

		//int get_flag();

		//void add_inFP();
		//void search_inFP();
	private:
		

		
};
/*******************************************************************************************************/
/****************************  Call Back Interface END declairation   **********************************/
/*******************************************************************************************************/

/************************************  Fingerprinter Class declairation  ******************************************/
class FingerPrinter
{

	public:		
		/*************************************************************************************************/
		/****************************     Call Back declairation        **********************************/
		/*************************************************************************************************/		
		
		/**
		 * Registers the callback which is called whenever there is a sample.
		 * \param cb Pointer to the callback interface.
		 **/
		void registerCallback(FingerPrint_CallBack* cb);

		/**
		 * Unregisters the callback to the callback interface.
		 **/
		void unRegisterCallback();

		/**
		 * Starts the data acquisition
		 **/
		void start();


		/**
		 * Stop the data acquisition
		 **/
		void stop();
				//Run the CB_Search func THREAD in callback style
		void Test1();
		
		//Run the CB_Add func THREAD in callback style
		void Test2();	

		/*************************************************************************************************/
		/****************************     Call Back declairation  end      *******************************/
		/*************************************************************************************************/
	protected:
			/**
		 * Starts the data acquisition
		 **/
		virtual void run1() = 0;
		virtual void run2() = 0;
	private:
		int running;
		thread* thread_1 = nullptr;
		thread* thread_2 = nullptr;
		FingerPrint_CallBack* fp_callback_ptr = nullptr;
		
		//callback get image
		bool CB_GetImage();
		
		//callback gen character
		bool CB_GenChar(uchar bufferID);
		
		//callback exit
		bool CB_Exit();
		
		//callback search
		bool CB_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
		
		//callback add
		void cb_add();
		
		
		static bool getimage_CALLBACK(void* lpvoid);
		
		static bool genchar_CALLBACK(void* lpvoid, uchar bufferID);
		
		static bool exit_CALLBACK(void* lpvoid);
		
		static bool search_CALLBACK(void* lpvoid, uchar bufferID, int startPageID, int count, int* pPageID, int* pScore); 
		
		

		
		static void exec1(FingerPrinter* exe){
			exe->run1();
			//wiringPiISR(SWITCH, INT_EDGE_RISING, exe->Test1());
		}
		static void exec2(FingerPrinter* exe){
			exe->run2();
		}
		
		/********************************* MAIN FUNCS **************************************/

		bool PS_Setup(uint chipAddr, uint password);       // 0x00000000 ~ 0xffffffff
		bool PS_VfyPwd(uint passwd);   // 4bits  Integer 
		bool PS_ReadSysPara();
		
		bool PS_GetImage();
		bool PS_GenChar(uchar bufferID);
		bool PS_Match(int* pScore);
		bool PS_RegModel();
		bool PS_StoreChar(uchar bufferID, int pageID);
		
		bool PS_DetectFinger();
		bool waitUntilDetectFinger(int wait_time);   // 阻塞至检测到手指，最长阻塞wait_time毫秒
		bool waitUntilNotDetectFinger(int wait_time);
		
		bool PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
		void lockerControl();
		
		bool PS_Empty();
		bool PS_Exit();
		// carry the g_error_code val and return to g_error_desc
		char* PS_GetErrorDesc();
		

		/********************************* HELPER FUNCS **************************************/
		void Split(uint num, uchar* buf, int count);
		//merge multi int val into one unsigned int val
		bool Merge(uint* num, const uchar* startAddr, int count);
		// print hex buffer
		void PrintBuf(const uchar* buf, int size);
		//calculate the check summation
		int Calibrate(const uchar* buf, int size);
		//Judge the confirmation code && calculate the check summation
		bool Check(const uchar* buf, int size);
		//send instruction package
		int SendOrder(const uchar* order, int size);
		//recive reply package
		bool RecvReply(uchar* hex, int size);
		// shoe the process percentage
		void PrintProcess(int done, int all);
		//Receive packet Acknowledgment code
		bool RecvPacket(uchar* pData, int validDataSize);
		//Send packet Acknowledgment code
		bool SendPacket(uchar* pData, int validDataSize);
		//Constructs a directive package, the result of which is assigned to the global variable g_order
		int GenOrder(uchar orderCode, const char* fmt, ...);

		
		static void atExitFunc();
		void printConfig();
		bool readConfig(); 
		void asyncConfig();
		bool writeConfig();
		
		/********************************* OTHER FUNCS **************************************/
		//////////////////////utils////////////////////////////////////
		//remove all the white space in file
		void trimSpaceInFile(const char* filename);

		// remove spave and '\n' in string
		void trim(const char* strIn, char* strOut);

		//convert string to int
		int toInt(const char* str);

		//convert Hexadecimal char* to unit
		unsigned int toUInt(const char* str);
	
		/*
		bool PS_LoadChar(uchar bufferID, int pageID);
		bool PS_UpChar(uchar bufferID, const char* filename);
		bool PS_DownChar(uchar bufferID, const char* filename);
		bool PS_UpImage(const char* filename);
		bool PS_DownImage(const char* filename);
		bool PS_DeleteChar(int startpageID, int count);
		bool PS_WriteReg(int regID, int value);
		bool PS_ReadSysPara();
		bool PS_Enroll(int* pPageID);
		bool PS_Identify(int* pPageID, int* pScore);
		bool PS_SetPwd(uint passwd);   // 4bits  Integer 
		bool PS_VfyPwd(uint passwd);   // 4bits  Integer 
		bool PS_GetRandomCode(uint* pRandom);
		bool PS_SetChipAddr(uint newAddr);
		bool PS_ReadINFpage(uchar* pInfo, int size);
		bool PS_WriteNotepad(int notePageID, uchar* pContent, int contentSize);
		bool PS_ReadNotepad(int notePageID, uchar* pContent, int contentSize);
		bool PS_HighSpeedSearch(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
		bool PS_ValidTempleteNum(int* pValidN);
		bool PS_ReadIndexTable(int* indexList, int size);
		bool PS_SetBaudRate(int value);
		bool PS_SetSecureLevel(int level);
		bool PS_SetPacketSize(int size);
		bool PS_GetAllInfo();
		bool PS_Flush();
		*/
		
	};

/****************************  Fingerprinter declairation end   **********************************/


#ifdef __cplusplus
}
#endif
#endif // __EXECUTIVE_H__

