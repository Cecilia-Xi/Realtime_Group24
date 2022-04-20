#include "Executive.h"
#include <iostream>


  
class FINGERPRINT_Example_Callback : public FingerPrint_CallBack {
	Executive* exe;
	virtual void demo_func(int a) {
		printf("%d\n",a);
	}	
	virtual void demo_func2(int a) {
		printf("%d\n",a);
	}
	
	virtual void demo_search() {
	printf("Please put your finger on the module.\n");
	exe->PS_GetImage() || exe->PS_Exit();
	exe->PS_GenChar(1) || exe->PS_Exit();

	int pageID = 0, score = 0;
	if (!exe->PS_Search(1, 0, 300, &pageID, &score))
	  exe->PS_Exit();
	else
	  printf("Matched! pageID=%d score=%d\n", pageID, score);
	}   
		
};

//extern int a1;
int main(int argc, char *argv[]) //int serialOpen (const char *device, const int baud)
{	
	
	
	Executive e;
	e.start();
	getchar();
	e.stop();
	/*
	Executive exe1;
	exe1.old_run();
	*/


	return 0;
}

