#ifndef _BITRUN_H_
#define _BITRUN_H_
#include <stdint.h>
#include <stdbool.h>
#define BITRUN_TYPE uint64_t
typedef struct bitRun{
	BITRUN_TYPE * run;
	int length;
} Bitrun;
Bitrun * shiftAndAdd(Bitrun * bitrun,bool bit);
Bitrun * createBitRun(Bitrun * run);

#endif // _BITRUN_