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

  uint detect_pin;      // AS608的WAK引脚连接的树莓派GPIO引脚号
  uint has_password;    // 是否有密码
} AS_608;
/********************type construct end******************/

/********************Global Variables******************/
extern AS_608 g_as608;
extern int   g_fd;          // 文件描述符，即open()函数打开串口的返回值
extern int   g_verbose;     // 输出信息的详细程度
extern char  g_error_desc[128]; // 错误代码的含义
extern uchar g_error_code;      // 模块返回的确认码，如果函数返回值不为true，读取此变量

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
	
	/*
	 * 辅助函数
	 * 计算检校和(从buf的第7字节开始计算，一直到倒数第三字节，计算每一字节的数值之和)
	**/
	int Calibrate(const uchar* buf, int size);
	
	/*
	 * 辅助函数
	 * 判断确认码 && 计算检校和
	 * 参数：buf，模块的应答包数据
	 *     size，应答包的有效字节数
	*/
	bool Check(const uchar* buf, int size);
	
	/* 
	 *  辅助函数
	 *  发送指令包
	 *  参数，size(实际准备发送的有效字符，不包含结尾的'\0'.  ！！！)
	*/
	int SendOrder(const uchar* order, int size);
	
	/* 
	 *  辅助函数
	 *  接收应答包
	 *  参数：size(实际准备接收的数据，包括指令头、包长度、数据区、检校和等)
	 */
	bool RecvReply(uchar* hex, int size);
	
	/*
	 * 显示进度条
	 * 参数：done(已完成的量)  all(总量)
	*/
	void PrintProcess(int done, int all);
	
	/* 
	 *  辅助函数
	 *  接收数据包 确认码0x02表示数据包且有后续包，0x08表示最后一个数据包
	 *  参数：
	 *     validDataSize表示有效的数据大小，不包括数据头、检校和部分
	*/
	bool RecvPacket(uchar* pData, int validDataSize);
	
	/* 
	 *  辅助函数
	 *  发送数据包 确认码0x02表示数据包且有后续包，0x08表示最后一个数据包
	 *  参数：
	 *     validDataSize表示有效的数据大小，不包括数据头、检校和部分
	*/
	bool SendPacket(uchar* pData, int validDataSize);
	
	/*
	 * 辅助函数
	 * 构造指令包，结果赋值给全局变量 g_order
	 * 参数：
	 *   orderCode, 指令代码， 如0x01, 0x02, 0x1a...
	 *   fmt, 参数描述，例如指令包带有2个参数，一个uchar类型，占1个字节，另一个uchar类型，占2个字节
	 *          那么fmt应为"%1d%2d"。
	 *      如果参数为一个uchar类型，占1个字节，另一个为uchar*类型，占32个字节
	 *          那么fmt应为"%d%32s"
	 *      (数字代表该参数占的字节数，为1则可忽略，字母代表类型, 只支持%d、%u 和 %s)
	*/
	int GenOrder(uchar orderCode, const char* fmt, ...);
	
	
};

#endif // __AS608_H__
