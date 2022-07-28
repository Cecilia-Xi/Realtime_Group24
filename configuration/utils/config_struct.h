#ifndef CONFIG_STRUCT_H
#define CONFIG_STRUCT_H

#include <stdbool.h>

typedef struct _Configuration {
  unsigned int address;
  unsigned int password;
  int has_password;
  int baudrate;
  int detect_pin;
  char serial[16];
} Configuration;

#endif

