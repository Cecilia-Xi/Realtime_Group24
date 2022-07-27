#ifndef TYPE_STRUCT_H
#define TYPE_STRUCT_H

#include <stdbool.h>

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct _FP_sensor_modle {
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





/*
extern int auto_page_id;

extern FP_sensor g_as608;
extern Configuration g_config;  

extern int   g_fd;          // 文件描述符，即open()函数打开串口的返回值
extern int   g_verbose;     // 输出信息的详细程度
extern char  g_error_desc[128]; // 错误代码的含义
extern uchar g_error_code;      // 模块返回的确认码，如果函数返回值不为true，读取此变量

extern uchar g_order[64]; // 发送给模块的指令包
extern uchar g_reply[64]; // 模块的应答包
* 
* 
*/
//#include "sensor_modle.cpp"
#endif
