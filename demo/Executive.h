#ifndef EXECUTIVE_H_
#define EXECUTIVE_H_

#include "AS608.h"
#include "Utils.h"

#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

extern AS_608 g_as608;
extern int g_fd;
extern int g_verbose;
extern char  g_error_desc[128];
extern uchar g_error_code;

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

class Executive{
	public:

		int  g_argc = 0;   // numbers of parameters，g_argc = argc - g_option_count
		int  g_option_count = 0; // numbers of options-v、-h etc.
		char g_command[16] = { 0 };     // that is argv[1]
		Config g_config;   // Configuration file structure, defined in the "./utils.h" header file

		
		Executive();
		~Executive();
		void run(int argc, char *argv[]);

		void printConfig();
		void printUsage();
		bool readConfig();  // read file to g_config
		bool writeConfig(); // wirte g_config into file
		void asyncConfig();
		void priorAnalyseArgv(int argc, char* argv[]);
		void analyseArgv(int argc, char* argv[]);

		bool waitUntilDetectFinger(int wait_time);   // Block until a finger is detected, the longest block wait_time milliseconds
		bool waitUntilNotDetectFinger(int wait_time);
		bool PS_Exit();
		static void atExitFunc();
		bool checkArgc(int argcNum);
		bool match(const char* str);
		
		void lockerControl();
		
	private:
	
		// Callback function pointers
		Car* a1 = nullptr;
		
		Car car1;
		Bike bike1;
	};
#ifdef __cplusplus
}
#endif
#endif // __EXECUTIVE_H__
