#ifndef FINGER_PRINT_H
#define FINGER_PRINT_H
#include <cstring>
#include <cstdio>
#include <math.h>
using namespace std;

#include "lib/lib.h"
#include "../configuration/utils/utils.h"

#define MAX_BUF 512
#define SYSFS_GPIO_DIR "/sys/class/gpio"



//Callback calss for Executive get result of search function and add function
class CallBack{
  public:
    /*
    * intro: prepared virtual adding result func, define in Executive later
    * param: added finger id, matched score
    * return: void,print message only
    */
    virtual void checkADD(int finger, int score)  const = 0;

    /*
    * intro: prepared virtual search result func, define in Executive later
    * param: detected finger id
    * return: void,print message only
    */
	virtual void checkSEARCH(int finger)  const = 0;
};




class FingerPrint : public Lib {
	
public:		

    int auto_page_id = 0; //automatically increased user id, +1 every add() executed
    uchar g_reply[64] = { 0 }; // sensor reply package

    /*
    * intro: copy constructor for private variable m_CallBack
    * param: copy constructor pointer
    * return: none
    */
	FingerPrint(CallBack* mCopy_CallBack);

    /*
    * intro:  destructor  delete
    * param: none
    * return: none
    */
	~FingerPrint();

    /*
    * intro:  encapsulation funtion for search the fingerprint in the sensor library
    * param: none
    * return: true if the fingerprint existed, otherwise false
    */
	bool search();

    /*
    * intro:  encapsulation funtion for add a new fingerprint into sensor library
    * param: none
    * return: none
    */
	void add();

    /*
    * intro:  setting up fingerprint srtuct variable with configuration get from fingerprint sensor
    * param: none
    * return: none
    */
	bool setUp(uint chipAddr, uint password);       // 0x00000000 ~ 0xffffffff

    /*
    * intro: detect is a finger on the sensor screen
    * param: none
    * return: true if find finger
    */
	bool PS_DetectFinger();

    /*
    * intro: free memory allocation, execute in destructor
    * param: none
    * return: none
    */
	void atExitFunc();
	
private:

    CallBack* m_CallBack;//callback pointer

    /*
     *  private member function
    * intro: detect is a finger on the sensor screen
    * param: none
    * return: true if succeed
    */
	bool PS_VfyPwd(uint passwd);   // 4bits  Integer 

    /*
     *  private member function
    * intro: detect is a finger on the sensor screen
    * param: none
    * return: true if succeed
    */
	bool PS_ReadSysPara();

    /*
     *  private member function
    * intro: detect is a finger on the sensor screen
    * param: none
    * return: true if succeed
    */
	bool PS_Search(uchar bufferID, int startPageID, int count, int* pPageID, int* pScore);

    /*
     *  private member function
    * intro: detect is a finger on the sensor screen
    * param: none
    * return: true if succeed
    */
    bool PS_Empty();


    /*
     *  private member function
    * intro: get finger image , prepared for compare in PS_Match()
    * param: none
    * return: true if succeed
    */
	bool PS_GetImage();

    /*
     *  private member function
    * intro: used in GetImage() to generate finger feature
    * param: bufferID
    * return: true if succeed
    */
	bool PS_GenChar(uchar bufferID);

    /*
     *  private member function
    * intro: Compare two images get from two times GetImage,
    * param: none
    * return: true if get result of the fingerprint score
    */
	bool PS_Match(int* pScore);

    /*
     *  private member function
    * intro: merge two fingerprint images to a model
    * param: none
    * return: true if succeed
    */
	bool PS_RegModel();

    /*
     *  private member function
    * intro: store the saved model of the buffer to the pageid into sensor library
    * param: none
    * return: true if succeed
    */
	bool PS_StoreChar(uchar bufferID, int pageID);
	
    /*
     *  private member function
    * intro: wait with wait_time second to detect finger is on the sensor
    * param: wait_time microSecond
    * return: true if detect finger
    */
    bool waitUntilDetectFinger(int wait_time);

    /*
     *  private member function
    * intro: wait with wait_time second to detect finger is NOT on the sensor
    * param: wait_time microSecond
    * return: true if not detect finger
    */
	bool waitUntilNotDetectFinger(int wait_time);

    /*
     * private member function
    * intro: used to terminate program and print debug code
    * param: none
    * return: true if get error
    */
	bool PS_Exit();

    /*
     * private member function
    * intro: carry the g_error_code value, coop with PS_Exit()
    * param: none
    * return: g_error_desc
    */
	char* PS_GetErrorDesc();
};

#endif
