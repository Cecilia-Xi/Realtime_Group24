#ifndef __CPP_THREAD_H_
#define __CPP_THREAD_H_

/**
 * GNU GENERAL PUBLIC LICENSE
 * Version 3, 29 June 2007
 *
 * (C) 2020-2022, Bernd Porr <mail@bernporr.me.uk>
 **/

#include <thread>

/**
 * A thin wrapper around the C++ thread model to avoid
 * a static callback. Instead just inherit this class
 * and overload run() which then runs in this thread.
 * This is header-only so that it can be performed
 * inline for max performance.
 **/
class CppThread {

public:
	/**
	 * Starts the thread.
	 **/
    inline void start_Search() {
		if (nullptr == uthread) {
            uthread = new std::thread(CppThread::exec_search, this);
		}
	}

    inline void start_Add() {
        if (nullptr == uthread) {
            uthread = new std::thread(CppThread::exec_add, this);
        }
    }
	/**
	 * Waits for the thread to terminate.
	 **/
	inline void join() {
		if (nullptr != uthread) {
			uthread->join();
			delete uthread;
			uthread = nullptr;
		}
	}

protected:
	/**
	 * This method does all the work of this thread.
         * Overload this abstract function with 
	 * a real one doing the actual work of this thread.
	 **/
    virtual void search_CppThread() = 0;
    virtual void add_CppThread() = 0;


private:
	// pointer to the thread
	std::thread* uthread = nullptr;

	// static function which points back to the instance
    static void exec_search(CppThread* cppThread) {
        cppThread->search_CppThread();
	}
    static void exec_add(CppThread* cppThread) {
        cppThread->add_CppThread();
    }
};


#endif
