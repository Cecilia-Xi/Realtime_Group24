#ifndef LIB_H
#define LIB_H

#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>
using namespace std;

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct _FP_sensor_modle
{
  uint status;      // Status registers 0
  uint model;       // sencor type 0-15
  uint capacity;    // fingerprint capacity，300
  uint secure_level;    // safety level 1/2/3/4/5，default =3 
  uint packet_size;     // data package size 32/64/128/256 bytes，default = 128
  uint baud_rate;       // baudrate val 
  uint chip_addr;                      
  uint password;        
  char product_sn[12];        // product module number
  char software_version[12];  // software edition number
  char manufacture[12];
  char sensor_name[12];

  uint detect_pin;      // AS608 WAK pins with raspi gpios
  uint has_password;  
} FP_sensor;

typedef struct _Configuration {
  unsigned int address;
  unsigned int password;
  int has_password;
  int baudrate;
  int detect_pin;
  char serial[16];
} Configuration;


class Lib
{
	public:
/*
 * inheri: father
 * intro: used to calculate address,
 *  num=0xa0b1c2d3，change to 0x0a, 0x1b, 0xc2, 0xd3
 * param: num, buffer, width
 * return: none
*/
/*
  * 继承人：父亲
  * intro：用于计算地址,
  *   num=0xa0b1c2d3，改为0x0a, 0x1b, 0xc2, 0xd3
  * 参数：整数，缓冲，宽度
  * 返回：无
*/
void Split(uint num, uchar* buf, int count);

/*
* inheri: father
* intro: used to calculate address,
*  num=xa0, 0xb1, 0xc2, 0xd3, merge to ee0xa0b1c2d3
* param: num, address, width
* return: true after merge succeed
*/
/*
* 继承人：父亲
* intro：用于计算地址，
*   num=xa0, 0xb1, 0xc2, 0xd3, 合并到ee0xa0b1c2d3
* 参数：无符号整数指针、无符号字符指针 地址、宽度
* 返回：合并成功后为真
*/
bool Merge(uint* num, const uchar* startAddr, int count);

/*
  * 继承人：父亲
  * 介绍：用于计算缓冲区
  * 参数：缓冲区，宽度
  * 返回：缓冲区的总和
*/
/*
 * inheri: father
 * intro: used to calculate buffer sum
 * param: buffer, width
 * return: sum of buffer
*/
int Calibrate(const uchar* buf, int size);
	private:
	
	};
  


#endif
