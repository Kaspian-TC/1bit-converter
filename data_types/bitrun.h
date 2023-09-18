#ifndef _BITRUN_H_
#define _BITRUN_H_
#include <stdint.h>
#include <stdbool.h>
#define BITRUN_TYPE uint64_t
typedef struct bitRun{
	BITRUN_TYPE * run;
	int length;
} Bitrun;
void freeBitrun(Bitrun * bitrun);
Bitrun * shiftAndAdd(Bitrun * bitrun, bool bit);
Bitrun * createBitRun(void);

#endif // _BITRUN_