#ifndef FINGER_PRINT_H
#define FINGER_PRINT_H

class FingerPrint
{

	public:		
		FingerPrinter();
		~FingerPrinter();

	private:

		/********************************* MAIN FUNCS **************************************/

		bool PS_Setup(uint chipAddr, uint password);       // 0x00000000 ~ 0xffffffff
		bool PS_VfyPwd(uint passwd);   // 4bits  Integer 
		bool PS_ReadSysPara();
		
		bool PS_GetImage();
		bool PS_GenChar(uchar bufferID);
		bool PS_Match(int* pScore);
		bool PS_RegModel();
		bool PS_StoreChar(uchar bufferID, int pageID);
		
		//bool PS_DetectFinger();
		bool waitUntilDetectFinger(int wait_time);   // 阻塞至检测到手指，最长阻塞wait_time毫秒
		bool waitUntilNotDetectFinger(int wait_time);
		
		bool PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);
		void lockerControl();
		
		bool PS_Empty();
		bool PS_Exit();
		// carry the g_error_code val and return to g_error_desc
		char* PS_GetErrorDesc();
		
		
		void atExitFunc();
		void printConfig();
		bool readConfig(); 
		void asyncConfig();
		bool writeConfig();
		


		
	};

/****************************  Fingerprinter declairation end   **********************************/

#endif
