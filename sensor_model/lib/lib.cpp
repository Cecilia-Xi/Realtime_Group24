#include "lib.h"

void Lib::Split(uint num, uchar* buf, int count) {
  for (int i = 0; i < count; ++i) {
    *buf++ = (num & 0xff << 8*(count-i-1)) >> 8*(count-i-1);
  }
}
bool Lib::Merge(uint* num, const uchar* startAddr, int count) {

  *num = 0;
  for (int i = 0; i < count; ++i)
    *num += (int)(startAddr[i]) << (8*(count-i-1)); 

  return true;
}
int Lib::Calibrate(const uchar* buf, int size) {

  int count = 0;
  for (int i = 6; i < size - 2; ++i) {
    count += buf[i];
  }

  return count;
}


bool Lib::Check(const uchar* buf, int size) {
  int count_ = 0;       // 模块传来的检校和 
  Merge((uint*)&count_, buf+size-2, 2); 

  // 自己计算的检校和
  int count = Calibrate(buf, size);

  return (buf[9] == 0x00 && 
          count_ == count && 
          buf[0] != 0x00);   // 防止全为0x00
}

int Lib::SendOrder(const uchar* order, int size) {
  // 输出详细信息
  if (g_verbose == 1) {
    printf("sent: ");
    PrintBuf(order, size);
  }
  int ret = write(g_fd, order, size);
  return ret;
}

bool Lib::RecvReply(uchar* hex, int size) {
  int availCount = 0;
  int timeCount  = 0;

  while (true) {
    if (serialDataAvail(g_fd)) {
      hex[availCount] = serialGetchar(g_fd);
      availCount++;
      if (availCount >= size) {
        break;
      }
    }
    //delay for wait until get all reply back, otherwise break and return false
    usleep(10); // 等待10微秒
    timeCount++;
    if (timeCount > 300000) {   // 最长阻塞3秒
      break;
    }
  }

  // 输出详细信息
  if (g_verbose == 1) {
    printf("recv: ");
    PrintBuf(hex, availCount);
  }

  // 最大阻塞时间内未接受到指定大小的数据，返回false
  if (availCount < size) {
    g_error_code = 0xff;
    return false;
  }

  g_error_code = hex[9];
  return true;
}


bool Lib::SendPacket(uchar* pData, int validDataSize) {
  if (g_as608.packet_size <= 0)
    return false;
  if (validDataSize % g_as608.packet_size != 0) {
    g_error_code = 0xC8;
    return false;
  }
  int realPacketSize = 11 + g_as608.packet_size; // 实际每个数据包的大小
  int realDataSize = validDataSize * realPacketSize / g_as608.packet_size;  // 总共需要发送的数据大小

  // 构造数据包
  uchar* writeBuf = (uchar*)malloc(realPacketSize);
  writeBuf[0] = 0xef;  // 包头
  writeBuf[1] = 0x01;  // 包头
  Split(g_as608.chip_addr, writeBuf+2, 4);  // 芯片地址
  Split(g_as608.packet_size+2, writeBuf+7, 2);  // 包长度

  int offset     = 0;  // 已发送的有效数据
  int writeCount = 0;  // 已发送的实际数据

  while (true) {
    // 填充数据区域
    memcpy(writeBuf+9, pData+offset, g_as608.packet_size);

    // 数据包 标志
    if (offset + g_as608.packet_size < validDataSize)
      writeBuf[6] = 0x02;  // 结束包(最后一个数据包)
    else
      writeBuf[6] = 0x08;  // 普通数据包

    // 检校和
    Split(Calibrate(writeBuf, realPacketSize), writeBuf+realPacketSize-2, 2); 

    // 发送数据包
    write(g_fd, writeBuf, realPacketSize);

    offset     += g_as608.packet_size;
    writeCount += realPacketSize;

    // 是否输出详细信息
    if (g_verbose == 1) {
      printf("%2d%% SentData: %d  count=%4d/%-4d  ", 
          (int)((double)writeCount/realDataSize*100), realPacketSize, writeCount, realDataSize);
      PrintBuf(writeBuf, realPacketSize);
    }
    else if (g_verbose == 0) {
      // 显示进度条
      PrintProcess(writeCount, realDataSize);
    }
    else {
      // show nothing
    }

    if (offset >= validDataSize)
      break;
  } // end while

  free(writeBuf);
  g_error_code = 0x00;
  return true; 
}

bool Lib::RecvPacket(uchar* pData, int validDataSize) {
  if (g_as608.packet_size <= 0)
    return false;
  int realPacketSize = 11 + g_as608.packet_size; // 实际每个数据包的大小
  int realDataSize = validDataSize * realPacketSize / g_as608.packet_size;  // 总共需要接受的数据大小

  uchar readBufTmp[8] = { 0 };  // 每次read至多8个字节，追加到readBuf中
  uchar* readBuf = (uchar*)malloc(realPacketSize); // 收满realPacketSize字节，说明收到了一个完整的数据包，追加到pData中

  int availSize      = 0;
  int readSize       = 0;
  int readCount      = 0;

  int readBufSize    = 0;
  int offset         = 0;
  int timeCount      = 0;
  
  while (true) {
    if ((availSize = serialDataAvail(g_fd)) > 0) {
      timeCount = 0;
      if (availSize > 8) {
        availSize = 8;
      }
      if (readBufSize + availSize > realPacketSize) {
        availSize = realPacketSize - readBufSize;
      }

      memset(readBufTmp, 0, 8);
      readSize = read(g_fd, readBufTmp, availSize);
      memcpy(readBuf+readBufSize, readBufTmp, readSize);

      readBufSize += readSize;
      readCount   += readSize;

      // 是否输出详细信息
      if (g_verbose == 1) {
        printf("%2d%% RecvData: %d  count=%4d/%-4d  ", 
            (int)((double)readCount/realDataSize*100), readSize, readCount, realDataSize);
        PrintBuf(readBufTmp, readSize);
      }
      else if (g_verbose == 0){ // 默认显示进度条
        PrintProcess(readCount, realDataSize);
      }
      else {
        // show nothing
      }

      // 接收完一个完整的数据包(139 bytes)
      if (readBufSize >= realPacketSize) {
        int count_ = 0;
        Merge((uint*)&count_, readBuf+realPacketSize-2, 2);
        if (Calibrate(readBuf, realPacketSize) != count_) {
          free(readBuf);
          g_error_code = 0x01;
          return false;
        }

        memcpy(pData+offset, readBuf+9, g_as608.packet_size);
        offset += g_as608.packet_size;
        readBufSize = 0;

        // 收到 结束包
        if (readBuf[6] == 0x08) {
          break;
        }
      }

      // 接受到 validDataSize 个字节的有效数据，但仍未收到结束包，
      if (readCount >= realDataSize) {
        free(readBuf);
        g_error_code = 0xC4;
        return false;
      }
    } // end outer if

    usleep(10); // 等待10微秒
    timeCount++;
    if (timeCount > 300000) {   // 最长阻塞3秒
      break;
    }
  } // end while

  free(readBuf);

  // 最大阻塞时间内未接受到指定大小的数据，返回false
  if (readCount < realDataSize) {
    g_error_code = 0xC3;
    return false;
  }
  
  g_error_code = 0x00;
  return true; 
}

int Lib::GenOrder(uchar orderCode, const char* fmt, ...) {
  g_order[0] = 0xef;        // 包头，0xef
  g_order[1] = 0x01;
  Split(g_as608.chip_addr, g_order+2, 4);    // 芯片地址，需要使用PS_Setup()初始化设置
  g_order[6] = 0x01;        // 包标识，0x01代表是指令包，0x02数据包，0x08结束包(最后一个数据包)
  g_order[9] = orderCode;   // 指令


  // 计算 参数总个数
  int count = 0;
  for (const char* p = fmt; *p; ++p) {
    if (*p == '%')
      count++;
  }

  // fmt==""
  if (count == 0) { 
    Split(0x03, g_order+7,  2);  // 包长度
    Split(Calibrate(g_order, 0x0c), g_order+10, 2);  // 检校和(如果不带参数，指令包长度为12，即0x0c)
    return 0x0c;
  }
  else {
    va_list ap;
    va_start(ap, fmt);

    uint  uintVal;
    uchar ucharVal;
    uchar* strVal;

    int offset = 10;  // g_order指针偏移量
    int width = 1;    // fmt中修饰符的宽度，如%4d, %32s

    // 处理不定参数
    for (; *fmt; ++fmt) {
      width = 1;
      if (*fmt == '%') {
        const char* tmp = fmt+1;

        // 获取宽度，如 %4u, %32s
        if (*tmp >= '0' && *tmp <= '9') {
          width = 0;
          do {
            width = (*tmp - '0') + width * 10;
            tmp++;
          } while(*tmp >= '0' && *tmp <= '9');
        }

        switch (*tmp) {
          case 'u':
          case 'd':
            if (width > 4)
              return 0;
            uintVal = va_arg(ap, int);
            Split(uintVal, g_order+offset, width);
            break;
          case 'c': // 等价于"%d"
            if (width > 1)
              return 0;
            ucharVal = va_arg(ap, int);
            g_order[offset] = ucharVal;
            break;
          case 's':
            strVal = va_arg(ap, uchar*);
            memcpy(g_order+offset, (char*)strVal, width);
            break;
          default:
            return 0;
        } // end switch 

        offset += width;
      } // end if (*p == '%')
    } // end for 

    Split(offset+2-9, g_order+7, 2);  // 包长度
    Split(Calibrate(g_order, offset+2), g_order+offset, 2); // 检校和
    
    va_end(ap);
    return offset + 2;
  } // end else (count != 0)
}


void Lib::PrintProcess(int done, int all) {
  // 进度条，0~100%，50个字符
  char process[64] = { 0 };
  double stepSize = (double)all / 100;
  int doneStep = done / stepSize;
  
  // 如果步数是偶数，更新进度条
  // 因为用50个字符显示100个进度值
  for (int i = 0; i < doneStep / 2 - 1; ++i)
    process[i] = '=';
  process[doneStep/2 - 1] = '>';

  printf("\rProcess:[%-50s]%d%% ", process, doneStep);
  if (done < all)
    fflush(stdout);
  else
    printf("\n");
}

void Lib::PrintBuf(const uchar* buf, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%02X ", buf[i]);
  }
  printf("\n");
}
