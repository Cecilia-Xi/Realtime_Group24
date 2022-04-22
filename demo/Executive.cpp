#include "Executive.h"



/*************************************************************************************************/
/****************************          Call Back END            **********************************/
/*************************************************************************************************/


//int argc, char *argv[]

Executive::Executive()
{
  /**< Getter function pointer of type int void (*summer)(int,int). */


}
Executive::~Executive()
{
	
}

void Executive::run1()
{
  printf("testinggggggg     run1\n");
  while(is_Running)
  {
    delay(20);
    Test1();
    }
}

void Executive::run2()
{

  while(is_Running)
  {
    delay(100);
    Test2();
    }
}
void Executive::start_run()
{
  is_Running =1;
  printf("testinggggggg\n");
  start();
  }
void Executive::stop_run()
{
  is_Running = 0;
  stop();
}

int Executive::fd_Poll(int gpio_fd, int timeout)
{
  struct pollfd fdset[1];
  int nfds = 1;
  int rc;
  char buf[MAX_BUF];
  memset((void*)fdset, 0, sizeof(fdset));

  fdset[0].fd = gpio_fd;
  fdset[0].events = POLLPRI;//

  rc = poll(fdset, nfds, timeout);

  if (fdset[0].revents & POLLPRI) {//
    // dummy read
    //RecvPacket(uchar* pData, int validDataSize);

    int r = read(fdset[0].fd, buf, MAX_BUF);
    if (r < 0) {
#ifdef DEBUG
	perror("gpio/export");
#endif
      return r;
    }
    
    printf("TEST in revents");
  }
  printf("TEST end of fd poll");
  return rc;
}
