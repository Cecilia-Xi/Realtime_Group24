#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "sensor_model/finger_print.h"
//#include "configuration/config.h"
#include "configuration/utils/config_struct.h"
using namespace std;

class Executive : FingerPrint {
public:
	Configuration g_config;  
	
	Executive(){};
	~Executive(){};
	void run();
	void lockerControl();
private:

};
	
#endif
