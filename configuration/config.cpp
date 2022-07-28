#include "config.h"
/*
void Config::asyncConfig() {
  g_as608.detect_pin   = g_config.detect_pin;
  g_as608.has_password = g_config.has_password;
  g_as608.password     = g_config.password;
  g_as608.chip_addr    = g_config.address;
  g_as608.baud_rate    = g_config.baudrate;
}

bool Config::readConfig() {
  FILE* fp;

  
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  
  if (access(filename, F_OK) == 0) { 
    trimSpaceInFile(filename);
    fp = fopen(filename, "r");
  }
  else {
    
    g_config.address = 0xffffffff;
    g_config.password= 0x00000000;
    g_config.has_password = 0;
    g_config.baudrate = 9600;
    g_config.detect_pin = 1; 
    strcpy(g_config.serial, "/dev/ttyAMA0");

    writeConfig();

    printf("Please config the address and password in \"~/.fpconfig\"\n");
    printf("  fp cfgaddr   0x[address]\n");
    printf("  fp cfgpwd    0x[password]\n");
    printf("  fp cfgserial [serialFile]\n");
    printf("  fp cfgbaud   [rate]\n");
    printf("  fp cfgpin    [GPIO_pin]\n");
    return false;
  }

  char key[16] = { 0 };
  char value[16] = { 0 };
  char line[32] = { 0 };

  char *tmp;
  while (!feof(fp)) {
    fgets(line, 32, fp);
    
    
    if (tmp = strtok(line, "="))
      trim(tmp, key);
    else
      continue;
    if (tmp = strtok(NULL, "="))
      trim(tmp, value);
    else
      continue;
    while (!tmp)
      tmp = strtok(NULL, "=");

    int offset = 0;
    if (value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
      offset = 2;

    if (strcmp(key, "address") == 0) {
      g_config.address = toUInt(value+offset);
    }
    else if (strcmp(key, "password") == 0) {
      if (strcmp(value, "none") == 0 || strcmp(value, "false") == 0) {
        g_config.has_password = 0; 
      }
      else {
        g_config.has_password = 1; 
        g_config.password = toUInt(value+offset);
      }
    }
    else if (strcmp(key, "serial") == 0) {
      int len = strlen(value);
      if (value[len-1] == '\n')
        value[len-1] = 0;
      strcpy(g_config.serial, value);
    }
    else if (strcmp(key, "baudrate") == 0) {
      g_config.baudrate = toInt(value);
    }
    else if (strcmp(key, "detect_pin") == 0) {
      g_config.detect_pin = toInt(value);
    }
    else {
      printf("Unknown key:%s\n", key);
      fclose(fp);
      return false;
    }

  } // end while(!feof(fp))

  asyncConfig();

  fclose(fp);
  return true;
}

void Config::printConfig() {
  printf("address=%08x\n", g_config.address);
  if (g_config.has_password)
    printf("password=%08x\n", g_config.password);
  else
    printf("password=none(no password)\n");
  printf("serial_file=%s\n",   g_config.serial);
  printf("baudrate=%d\n",   g_config.baudrate);
  printf("detect_pin=%d\n",   g_config.detect_pin);
}



void Config::atExitFunc() {
  if (g_verbose == 1)
    printf("Exit\n");
  if (g_fd > 0)
    serialClose(g_fd); 
}


void Config::writeConfig() {
  
  char filename[256] = { 0 };
  sprintf(filename, "%s/.fpconfig", getenv("HOME"));
  
  FILE* fp  = fopen(filename, "w+");
  if (!fp) {
    printf("Write config file error!\n");
    exit(0);
  }

  fprintf(fp, "address=0x%08x\n", g_config.address);
  if (g_config.has_password)
    fprintf(fp, "password=0x%08x\n", g_config.password);
  else
    fprintf(fp, "password=none\n");
  fprintf(fp, "baudrate=%d\n", g_config.baudrate);
  fprintf(fp, "detect_pin=%d\n", g_config.detect_pin);
  fprintf(fp, "serial=%s\n", g_config.serial);

  fclose(fp);
}
*/
