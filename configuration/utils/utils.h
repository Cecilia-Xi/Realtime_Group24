#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdio.h>

 
// strip all whitespace characters (spaces and tabs) from the file
// only suitable for this example, for processing configuration files, the amount of data is small
void trimSpaceInFile(const char* filename);

// convert string to integer
// Remove leading and trailing spaces and newlines from the string
void trim(const char* strIn, char* strOut);

// convert string to integer
int toInt(const char* str);

// Convert *hex* string to unsigned int
unsigned int toUInt(const char* str);


#endif
