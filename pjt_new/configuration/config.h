#ifndef CONFIG_H
#define CONFIG_H

#include "../sensor_model/lib/lib.h"
#include "utils/utils.h"
using namespace std;

class Config  {
public:

	void atExitFunc();
	void printConfig();
	bool readConfig(); 
	void asyncConfig();
	void writeConfig();
private:

};
	
#endif
