#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <unistd.h>
#include <cstring>
#include <stdarg.h>

#include <wiringPi.h>
#include <wiringSerial.h>


#include "type_struct.h"

using namespace std;



class Lib {
  
public:

  FP_sensor g_as608;

  int   g_fd;          // 文件描述符，即open()函数打开串口的返回值
  int   g_verbose;     // 输出信息的详细程度
  char  g_error_desc[128]; // 错误代码的含义
  uchar g_error_code;      // 模块返回的确认码，如果函数返回值不为true，读取此变量
  uchar g_order[64] = { 0 }; // 发送给模块的指令包

  Lib(){};
  
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
   * 辅助函数
   * 构造指令包，结果赋值给全局变量 g_order
   * 参数：
   *   orderCode, 指令代码， 如0x01, 0x02, 0x1a...
   *   fmt, 参数描述，例如指令包带有2个参数，一个uchar类型，占1个字节，另一个uchar类型，占2个字节
   *          那么fmt应为"%1d%2d"。
   *      如果参数为一个uchar类型，占1个字节，另一个为uchar*类型，占32个字节
   *          那么fmt应为"%d%32s"
   *      (数字代表该参数占的字节数，为1则可忽略，字母代表类型, 只支持%d、%u 和 %s)
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
