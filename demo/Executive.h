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

int  g_argc = 0;   // 参数个数，g_argc = argc - g_option_count
int  g_option_count = 0; // 选项个数-v、-h等
char g_command[16] = { 0 };     // 即argv[1]
Config g_config;   // 配置文件 结构体，定义在"./utils.h"头文件中

class Executive{
	public:

		
		Executive();
		~Executive();
		void run(int argc, char *argv[]);

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
	private:
		Car car1;
		Bike bike1;
	};

#endif // __EXECUTIVE_H__
