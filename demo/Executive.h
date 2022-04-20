#ifndef EXECUTIVE_H_
#define EXECUTIVE_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <thread>
#include <stdbool.h>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>

#include <math.h>
#include <stdarg.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <iostream>
#include <fcntl.h>
#include <poll.h>
#include <string.h>

#ifndef NDEBUG
#define DEBUG
#endif

using namespace std;

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




#ifdef __cplusplus
extern "C" {
#endif
typedef struct _Config {
  unsigned int address;
  unsigned int password;
  int has_password;
  int baudrate;
  int detect_pin;
  char serial[16];
} Config;

	
	/****************************  Call Back INTERFACE declairation  **********************************/

/**
 * Callback for new samples which needs to be implemented by the main program.
 * The function hasSample needs to be overloaded in the main program.
 **/
 

	
class FingerPrint_CallBack {
	public:
		virtual void cb_func(int a) =0;
		virtual void cb_func2(int a) =0;
		virtual void cb_search() =0;
		
		
};


//bool PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);

// (return callback_param(lpvoid,a);)

typedef bool (*cb_search_ptr)(void*, uchar, int, int, int*, int*);
bool Get_search_CallBack(void* lpvoid, cb_search_ptr callback_param, uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);

typedef bool (*cb_getimage_ptr)(void*);
bool Get_getimage_CallBack(void* lpvoid, cb_getimage_ptr callback_param);

typedef bool (*cb_genchar_ptr)(void*, uchar);
bool Get_genchar_CallBack(void* lpvoid, cb_genchar_ptr callback_param, uchar a);

typedef bool (*cb_exit_ptr)(void*);
bool Get_exit_CallBack(void* lpvoid, cb_exit_ptr callback_param);


	/****************************            Call Back ENd          **********************************/


class Executive{
	public:
		//////utils////////////////////////////////////////////////////////////////////////////
		//remove all the white space in file
		void trimSpaceInFile(const char* filename);

		// remove spave and '\n' in string
		void trim(const char* strIn, char* strOut);

		//convert string to int
		int toInt(const char* str);

		//convert Hexadecimal char* to unit
		unsigned int toUInt(const char* str);

	
		//////AS608////////////////////////////////////////////////////////////////////////////
		bool PS_Setup(uint chipAddr, uint password);       // 0x00000000 ~ 0xffffffff
		
		bool PS_GetImage();
		bool PS_GenChar(uchar bufferID);
		bool PS_Match(int* pScore);
		bool PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
		bool PS_RegModel();
		bool PS_StoreChar(uchar bufferID, int pageID);
		bool PS_LoadChar(uchar bufferID, int pageID);
		bool PS_UpChar(uchar bufferID, const char* filename);
		bool PS_DownChar(uchar bufferID, const char* filename);
		bool PS_UpImage(const char* filename);
		bool PS_DownImage(const char* filename);
		bool PS_DeleteChar(int startpageID, int count);
		bool PS_Empty();
		bool PS_WriteReg(int regID, int value);
		bool PS_ReadSysPara();
		bool PS_Enroll(int* pPageID);
		bool PS_Identify(int* pPageID, int* pScore);
		bool PS_SetPwd(uint passwd);   // 4bits  Integer 
		bool PS_VfyPwd(uint passwd);   // 4bits  Integer 
		bool PS_GetRandomCode(uint* pRandom);
		bool PS_SetChipAddr(uint newAddr);
		bool PS_ReadINFpage(uchar* pInfo, int size/*>=512*/);
		bool PS_WriteNotepad(int notePageID, uchar* pContent, int contentSize);
		bool PS_ReadNotepad(int notePageID, uchar* pContent, int contentSize);
		bool PS_HighSpeedSearch(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
		bool PS_ValidTempleteNum(int* pValidN);
		bool PS_ReadIndexTable(int* indexList, int size);

		bool PS_DetectFinger();
		bool PS_SetBaudRate(int value);
		bool PS_SetSecureLevel(int level);
		bool PS_SetPacketSize(int size);
		bool PS_GetAllInfo();
		bool PS_Flush();

		// carry the g_error_code val and return to g_error_desc
		char* PS_GetErrorDesc();

	/////executive/////////////////////////////////////////////////////////////////////////////
	int  g_argc = 0;   // 参数个数，g_argc = argc - g_option_count
	int  g_option_count = 0; // 选项个数-v、-h等
	char g_command[16] = { 0 };     // 即argv[1]
	Config g_config;   // 配置文件 结构体，定义在"./utils.h"头文件中
	
	Executive();
	~Executive();
	void old_run();

	void printConfig();
	void printUsage();
	bool readConfig();  // 读取文件到 g_config
	bool writeConfig(); // 将 g_config 写入文件
	void asyncConfig();
	void priorAnalyseArgv(int argc, char* argv[]);
	void analyseArgv(int argc, char* argv[]);

	bool waitUntilDetectFinger(int wait_time);   // 阻塞至检测到手指，最长阻塞wait_time毫秒
	bool waitUntilNotDetectFinger(int wait_time);
	bool PS_Exit();
	static void atExitFunc();
	bool checkArgc(int argcNum);
	bool match(const char* str);
	
	void lockerControl();
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
		 * Stops the data acquistion
		 **/
		void stop();
		
			
		void Test()
		{
			while(1)
			{
				delay(1000);
				cb_search();
			}
		}
		
		bool cb_GetImage();
		bool cb_GenChar(uchar bufferID);
		bool cb_Exit();
		void cb_search();
		/*************************************************************************************************/
		/****************************     Call Back declairation  end      *******************************/
		/*************************************************************************************************/
		
	private:
		

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
	 
		/*************************************************************************************************/
		/****************************     Call Back declairation        **********************************/
		/*************************************************************************************************/
		int running;
		int fd = 0;
		thread* thread_1 = nullptr;
		FingerPrint_CallBack* fp_callback_ptr = nullptr;
		
		static bool getimage_CALLBACK(void* lpvoid);
		static bool genchar_CALLBACK(void* lpvoid, uchar bufferID);
		static bool exit_CALLBACK(void* lpvoid);
		
		static bool search_CALLBACK(void* lpvoid, uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);  
		
		void run();
		   
		static int fd_Poll(int gpio_fd, int timeout);
		
		static void exec(Executive* exe1){
			exe1->cb_search();
		}
		
		/*************************************************************************************************/
		/****************************          Call Back END            **********************************/
		/*************************************************************************************************/

	};
#ifdef __cplusplus
}
#endif
#endif // __EXECUTIVE_H__
