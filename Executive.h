#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "sensor_model/finger_print.h"
#include "configuration/utils/config_struct.h"
#include "cppThread/CppThread.h"
using namespace std;

/* callback interface
 * 
class SAMPLE_callback_interface {
public:
	virtual void hasSample(float sample) = 0;
};
 */
 

class Executive : public CppThread{
public:
	 /* callback registration
	 * 
	void registerCallback(SAMPLE_callback_interface* cb);
	
	//Unregisters the callback to the callback interface.
	void unRegisterCallback();

	 */
	Executive();
	~Executive();
	void initialize();
	
	void EXE_run();
	void run_plain();
	
	void search_withQT();
	void add_withQT();
	void lockerControl();

	void printConfig();
	bool readConfig(); 
	void asyncConfig();
	void writeConfig();

private:

	Config g_config;  
	FingerPrint g_fp;
	
	/* 
	 * callback run()
	 
	SAMPLE_callback_interface* sample_callback_interface_ptr = nullptr;
	void callback_run();
	static void exec(Executive* exe) { exe->callback_run(); }
	*/
};
	

/*
class Executive {
public:

	Executive();
	~Executive();
	
	void run();

	void initialize();
	void lockerControl();

	void printConfig();
	bool readConfig(); 
	void asyncConfig();
	void writeConfig();

private:

	Config g_config;  
	FingerPrint g_fp;

};
	*/
#endif
