#ifndef EXECUTIVE_H_
#define EXECUTIVE_H_
#include "FingerPrinter.h"

#ifdef __cplusplus
extern "C" {
#endif

class Executive: public FingerPrinter{
	public:
		Executive();
		~Executive();

		/**
		 * Stops the data acquistion
		 **/
		

		void start_run();
		/**
		 * Stops the data acquistion
		 **/
		void stop_run();
	protected:
		void run1(); 
		void run2(); 
	private:
		int is_Running;
		
		static int fd_Poll(int gpio_fd, int timeout);
	};
#ifdef __cplusplus
}
#endif

#endif // __EXECUTIVE_H__
