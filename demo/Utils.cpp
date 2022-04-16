#include "./utils.h"

void Bike::trimSpaceInFile(const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp)
    return;
  //buffer for input except white space and '\n'
  char lineBuf[64] = { 0 };
  char writeBuf[1024] = { 0 };
  
  int offset = 0;
  
  while (!feof(fp)) {
    fgets(lineBuf, 64, fp);
    if (feof(fp))
      break;
      
    for (int i = 0, len = strlen(lineBuf); i < len; ++i) {
      if (lineBuf[i] != ' ' && lineBuf[i] != '\t') {
        if (lineBuf[i] == '\n' && offset > 0 && writeBuf[offset-1] == '\n')
          continue;
        writeBuf[offset++] = lineBuf[i];
      }
    }
    
  }

  fclose(fp);
  //overwrtie file
  fp = fopen(filename, "w+");
  if (!fp)
    return;
  fwrite(writeBuf, 1, strlen(writeBuf), fp);
  fclose(fp);
}


void Bike::trim(const char* strIn, char* strOut) {
  int i = 0;
  int j = strlen(strIn) - 1;
  //detect space
  while (strIn[i] == ' ')
    ++i;
  //detect '\n'
  while (strIn[j] == ' ' || strIn[j] == '\n')
    --j;
  strncpy(strOut, strIn+i, j-i+1);
  strOut[j-i+1] = 0;
}


int Bike::toInt(const char* str) {
  int ret = 0;
  sscanf(str, "%d", &ret);
  return ret;
}

unsigned int Bike::toUInt(const char* str) {
  unsigned int ret = 0;
  if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    sscanf(str+2, "%x", &ret);
  else
    sscanf(str, "%x", &ret);

  return ret;
}
