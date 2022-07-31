#ifndef EXECUTIVE_H
#define EXECUTIVE_H
#include <QApplication>
#include <QThread>
#include <QObject>
#include "sensor_model/finger_print.h"
#include "configuration/utils/config_struct.h"
#include <QDebug>
//#include "cppThread/CppThread.h"

#define SWITCH 7
#define key_pin 29

using namespace std;


class Executive : public QObject, CallBack{
public:
    /* CALLBACK FUNCTION
     * virtual funtion declaration, inherite from CallBack class
     *
    * intro: print add result
    * param: finger id, score
    * return: none
    */
    void checkADD(int finger, int score) const;

    /* CALLBACK FUNCTION
     * virtual funtion declaration, inherite from CallBack class
     *
    * intro: print search result
    * param: finger id
    * return: none
    */
    void checkSEARCH(int finger) const;

    /*
     * public member function
    * intro: constructor
    * param: parent pointer
    * return: true if get error
    */
    explicit Executive(QObject *parent = nullptr);

    /*
     * public member function
    * intro: run in constructor,
    * first talk with as608 sensor
    * param: none
    * return:none
    */
    void initialize();

    /*
     * public member function
    * intro: destructor, free allocated memory
    * param: none
    * return: none
    */
	~Executive();

    /*
     * public member function
    * intro: used to change locker voltage with i seconds
    * param: second
    * return: none
    */
    void lockerControl(int second) const;

    /*
     * public member function
    * intro: demo run function in terminal
    * param: none
    * return: none
    */
    //void EXE_run();

    /*
     * public member function
    * intro: demo run with EXE_run in terminal
    * param: none
    * return: none
    */
    //void run_plain();

public slots:
    /*
     * public member slot function
    * intro: run m_fingerprint->search() in QTread, unlock the locker when m_fingerprint->search() return true
    * param: none
    * return: none
    */
    void search_withQT();

    /*
     * public member slot function
    * intro: run m_fingerprint->add() in QTread
    * param: none
    * return: none
    */
    void add_withQT();

private:
    Config g_config;//Configuration struct varible,used to set m_fingerprint->g_as608
    FingerPrint *m_fingerprint;//Fingerprint pointer

    /*
     * private member helper function
    * intro: config funciton, used in initialize()
    * param: none
    * return: none
    */
    void printConfig();

    /*
     * private member helper function
    * intro: config funciton, used in initialize()
    * param: none
    * return: none
    */
    bool readConfig();

    /*
     * private member helper function
    * intro: config funciton, used in initialize()
    * param: none
    * return: none
    */
    void asyncConfig();

    /*
     * private member helper function
    * intro: config funciton, used in initialize()
    * param: none
    * return: none
    */
    void writeConfig();

};
	
#endif
