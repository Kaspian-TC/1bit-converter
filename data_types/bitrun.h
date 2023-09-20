#ifndef _BITRUN_H_
#define _BITRUN_H_
#include <stdbool.h>
#include <stdint.h>
typedef struct bitRun Bitrun;
Bitrun * shiftAndAdd(Bitrun * bitrun, bool bit);
long assignBitrunToMemory(const Bitrun * bitrun,uint8_t * dest,long dest_start);
void freeBitrun(Bitrun * bitrun);
Bitrun * createAndCopyBitrun(const Bitrun * bitrun);
Bitrun * createBitRun(void);
#endif // _BITRUN_