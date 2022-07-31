#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <cstring>
#include <stdarg.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "type_struct.h"

using namespace std;



class Lib {
  
public:

  FP_sensor g_as608;//fingerprint sensor struct variale, wait to config with sensor data

  int   g_fd;          // folder describe character , the return value of wiringPI serial
  int   g_verbose;     // the oputput detail level
  char  g_error_desc[128]; // error code dedcription
  uchar g_error_code;      // sensor error code，if any funciton retule false, then get this value
  uchar g_order[64] = { 0 }; // the order package send to sensor

  /*
  * intro: constructor
  * param: num, buffer, width
  * return: none
  */
  Lib(){};

  /*
  * intro: destructor
  * param: none
  * return: none
  */
  ~Lib(){};
 
  /*
  * inheri: father
  * intro: used to calculate address,
  *  num=0xa0b1c2d3，change to 0x0a, 0x1b, 0xc2, 0xd3
  * param: num, buffer, width
  * return: none
  */
  void Split(uint num, uchar* buf, int count);

  /*
  * inheri: father
  * intro: used to calculate address,
  *  num=xa0, 0xb1, 0xc2, 0xd3, merge to ee0xa0b1c2d3
  * param: num, address, width
  * return: true after merge succeed
  */
  bool Merge(uint* num, const uchar* startAddr, int count);
  
  /*
  * delay: 0s
  * inheri: father
  * intro: used to calculate buffer sum
  * param: buffer, width
  * return: sum of buffer
  */
  int Calibrate(const uchar* buf, int size);
    
  /*
  * delay: 0s
  * inheri: father
  * intro: used to check buffer data, using Merge() and CAlibrate()
  * param: buffer, width
  * return: true if buffer compare is correct
  */
  bool Check(const uchar* buf, int size);
  
  /*
  * return nothing but int, change to void
  */
  /*
  * delay: 0s
  * inheri: father
  * intro: used to send order to sensor, work with RecvReply()
  * param: order, width
  * return: none
  */
  int SendOrder(const uchar* order, int size);
  
  /*
  * delay: max 3s wait for get reply
  * inheri: father
  * intro: used to get reply from sensor after send order, work with SendOrder()
  * param: hex data, width
  * return: false if delay too long or data over the required size
  */
  bool RecvReply(uchar* hex, int size);

  /*
  * delay: 0s
  * inheri: father
  * intro: used to send data packet to the senso, work with RecvPacket()
  * param: uchar data packet, width
  * return: true after finish
  */
  bool SendPacket(uchar* pData, int validDataSize);
  
  /*
  * delay: 0
  * inheri: father
  * intro: used to get data packet from the sensor, work with RecvPacket()
  * param: uchar data packet, width
  * return: true if get full packet back
  */
  bool RecvPacket(uchar* pData, int validDataSize);

  /*
  * inheri: father
  * intro: used to build order package
  * param: orderCode, fmt(a dynamic changed amount of variables )
  * return: set value to g_order
  */
  int GenOrder(uchar orderCode, const char* fmt, ...);

private:

  /*
  * delay: 0s
  * inheri: father
  * intro: process printer, (done/all) *100%
  * param: done, all
  * return: none
  */
  void PrintProcess(int done, int all);

  /*
  * delay: 0s
  * inheri: father
  * intro: debug printer
  * param: buffer, width
  * return: none
  */
  void PrintBuf(const uchar* buf, int size);
	
};
#endif
