#include "Executive.h"
#include <iostream>


  
class FINGERPRINT_Example_Callback : public FingerPrint_CallBack {
	virtual void demo_func(int a) {
		printf("%d\n",a);
	}	
	virtual void demo_func2(int a) {
		printf("%d\n",a);
	}
	

		
};

//extern int a1;
int main(int argc, char *argv[]) //int serialOpen (const char *device, const int baud)
{	
	
	
	Executive e;
	e.start_run();
	getchar();
	e.stop_run();
	e.stop();
	/*Executive exe1;
	exe1.old_run();
	*/


	return 0;
}

