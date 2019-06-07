#ifndef _FORMATTED_LOG_H_
#define _FORMATTED_LOG_H_

#ifdef DEBUG
#define DNDEBUG false
#else
#define DNDEBUG true
#endif

#include <stdio.h>
#include <stdarg.h>

// TODO: make more generic
inline char* cutFileName(char *txt) 
{
	// char *file_name = "src/";
	// return txt + 4;
	return strrchr(txt, '/');
}
#define formatted_log(msg, ...) \
	do {if (!DNDEBUG) fprintf(stderr, "\x1B[32mLOG: \033[0m%s[%d] (%s): " msg "\n", \
		cutFileName(__FILE__), __LINE__, __func__, ##__VA_ARGS__); } \
	while(0)

#define formatted_err(msg, ...) \
	do {if (!DNDEBUG) fprintf(stderr, "\x1B[31mERR: \033[0m%s[%d] (%s): " msg "\n", \
		cutFileName(__FILE__), __LINE__, __func__, ##__VA_ARGS__); } \
	while(0)

#endif