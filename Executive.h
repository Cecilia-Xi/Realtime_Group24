#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "sensor_model/finger_print.h"
#include "configuration/config.h"
#include "configuration/utils/config_struct.h"
using namespace std;

class Executive : public Config{
public:

	Executive();
	~Executive();
	
	void run();
	void initialize();
	void lockerControl();

	//void printConfig();
	//bool readConfig(); 
	//void asyncConfig();
	//void writeConfig();

private:
	Configuration g_config;  
	FingerPrint g_fp;
};

#endif
