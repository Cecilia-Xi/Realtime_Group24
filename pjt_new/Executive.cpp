#include "Executive.h"

void Executive::run(){
	FingerPrint f1;
	//f1.run();
}

void Executive::lockerControl()
{
  pinMode (SWITCH,OUTPUT);
  printf("FBI open The Door!\n");
  digitalWrite (SWITCH,LOW);//set it initially as high, is closed

  delay(1000);
  digitalWrite (SWITCH,HIGH);

  pinMode(g_config.detect_pin, INPUT);
}
