#ifndef AS608_H_
#define AS608_H_
#include <stdbool.h>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <wiringPi.h>
#include <wiringSerial.h>

/********************type construct******************/
	
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
/********************type construct end******************/

/********************Global Variables******************/
extern AS_608 g_as608;
extern int   g_fd; 
extern int   g_verbose;
extern char  g_error_desc[128];
extern uchar g_error_code;

/********************Global Variables ENd******************/

class Car
{
public:

	

	
	/********************Public Functions******************/
	Car(){};
	~Car(){};
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
	/********************Public Functions End******************/
private:

	//split unsigned int val into seperate int val
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
	
	
};

#endif // __AS608_H__
