#ifndef CONFIG_H
#define CONFIG_H

#include "../sensor_model/lib/lib.h"
#include "utils/utils.h"
#include "utils/config_struct.h"
#include "../sensor_model/finger_print.h"
using namespace std;

class Config : public FingerPrint  {
public:

	void atExitFunc();
	void printConfig();
	bool readConfig(); 
	void asyncConfig();
	void writeConfig();
private:

	Configuration g_config;  
	FingerPrint g_fp;
};
	
#endif
