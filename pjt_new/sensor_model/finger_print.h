#ifndef FINGER_PRINT_H
#define FINGER_PRINT_H

#include <cstring>
#include <cstdio>
#include <math.h>
using namespace std;

#include "lib/lib.h"
#include "../configuration/utils/utils.h"

#define MAX_BUF 512
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define key_pin 29
#define SWITCH 7

class FingerPrint : public Lib {
	
public:		
	int auto_page_id = 0;
	uchar g_reply[64]; // 模块的应答包
	
	FingerPrint();
	~FingerPrint();
	
	void run();
	void search();
	void add();
	bool setUp(uint chipAddr, uint password);       // 0x00000000 ~ 0xffffffff

	void atExitFunc();
	
private:
	
	bool PS_VfyPwd(uint passwd);   // 4bits  Integer 
	bool PS_ReadSysPara();
	
	bool PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
	
	bool PS_GetImage();
	bool PS_GenChar(uchar bufferID);
	bool PS_Match(int* pScore);
	bool PS_RegModel();
	bool PS_StoreChar(uchar bufferID, int pageID);
	bool PS_DetectFinger();
	
	bool waitUntilDetectFinger(int wait_time);   // 阻塞至检测到手指，最长阻塞wait_time毫秒
	bool waitUntilNotDetectFinger(int wait_time);

	bool PS_Empty();
	bool PS_Exit();

	// carry the g_error_code val and return to g_error_desc
	char* PS_GetErrorDesc();
};

#endif
