

#ifndef __UTILS_H__
#define __UTILS_H__
#include <string.h>
#include <stdio.h>
typedef struct _Config {
  unsigned int address;
  unsigned int password;
  int has_password;
  int baudrate;
  int detect_pin;
  char serial[16];
} Config;

class Bike
{
	public:
		Bike(){};
		~Bike(){};

		//remove all the white space in file
		void trimSpaceInFile(const char* filename);
		
		// remove spave and '\n' in string
		void trim(const char* strIn, char* strOut);
		
		//convert string to int
		int toInt(const char* str);

		//convert Hexadecimal char* to unit
		unsigned int toUInt(const char* str);

	private:
};


#endif // __UTILS_H__
