#include "Executive.h"
#include <iostream>

class GET_FingerPrint_CallBack: public FingerPrint_CallBack {
	public:

		virtual void cb_func1()
		{
			printf("-----done with search-----\n");
			}
		virtual void cb_func2()
		{
			printf("++++++done with add+++++\n");
			}

	private:
		

		
};

//extern int a1;
int main(int argc, char *argv[]) //int serialOpen (const char *device, const int baud)
{	
	 
	GET_FingerPrint_CallBack get_cb;
	Executive e;
	//e.wwww(qweqwe);
	e.registerCallback(&get_cb);
	e.start_run();
	getchar();
	e.stop_run();
	e.stop();
	/*Executive exe1;
	exe1.old_run();
	*/


	return 0;
}

