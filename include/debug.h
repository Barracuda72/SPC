#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
	Немного отладки
*/

#ifdef __DEBUG__

#include <stdio.h>
#define DBG(...) fprintf(stderr, __VA_ARGS__)

#else

#define DBG(...)

#endif //__DEBUG__

#endif //__DEBUG_H__
