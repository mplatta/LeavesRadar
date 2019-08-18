#ifndef _THREAD_INFO_HPP_
#define _THREAD_INFO_HPP_

#include "opencv_headers.hpp"
#include "formatted_log.hpp"

typedef struct Store_queue
{
	void (*func)(void *, void *);
	void *fir_data;
	void *sec_data;
} store_queue;

#endif
