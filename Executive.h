#ifndef EXECUTIVE_H
#define EXECUTIVE_H

#include "sensor_model/finger_print.h"
#include "configuration/utils/config_struct.h"
//#include "cppThread/CppThread.h"
#include <QThread>
#include <QObject>
using namespace std;
#define SWITCH 7
#define key_pin 29

class Executive : public QObject,CallBack{
public:

    explicit Executive(QObject *parent = nullptr);
	~Executive();

	void EXE_run();
	void run_plain();
	



	void checkADD(int finger, int score) const;
	void checkSEARCH(int finger) const;
		
	void initialize();
	void lockerControl();

	void printConfig();
	bool readConfig(); 
	void asyncConfig();
	void writeConfig();

public slots:
    void search_withQT();
    void add_withQT();

signals:
private:

	Config g_config;  
	FingerPrint *g_fp;

};
	
#endif
