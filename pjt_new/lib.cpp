
#include "lib.h"

void Split(uint num, uchar* buf, int count) {
  for (int i = 0; i < count; ++i) {
    *buf++ = (num & 0xff << 8*(count-i-1)) >> 8*(count-i-1);
  }
}

bool Merge(uint* num, const uchar* startAddr, int count) {
  *num = 0;
  for (int i = 0; i < count; ++i)
    *num += (int)(startAddr[i]) << (8*(count-i-1)); 

  return true;
}

int Calibrate(const uchar* buf, int size) {
  int count = 0;
  for (int i = 6; i < size - 2; ++i) {
    count += buf[i];
  }

  return count;
}
