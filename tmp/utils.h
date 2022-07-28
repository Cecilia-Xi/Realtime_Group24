#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdio.h>
/*
class Utils {
  public:
  	Utils(){};
    ~Utils(){};
    
    // 去除文件中所有空白字符(空格和制表符)
    // 仅适合本例，用于处理配置文件，数据量很小
    void trimSpaceInFile(const char* filename);

    // 把字符串转为整型
    // 去除字符串首尾空格 和 换行符
    void trim(const char* strIn, char* strOut);

    // 把字符串转为整型
    int toInt(const char* str);

    // 把 *十六进制* 字符串转为无符号整型
    unsigned int toUInt(const char* str);
  private:
  
};
*/
 
// 去除文件中所有空白字符(空格和制表符)
// 仅适合本例，用于处理配置文件，数据量很小
void trimSpaceInFile(const char* filename);

// 把字符串转为整型
// 去除字符串首尾空格 和 换行符
void trim(const char* strIn, char* strOut);

// 把字符串转为整型
int toInt(const char* str);

// 把 *十六进制* 字符串转为无符号整型
unsigned int toUInt(const char* str);


#endif
