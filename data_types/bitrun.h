#ifndef _BITRUN_H_
#define _BITRUN_H_
#include <stdbool.h>

typedef struct bitRun Bitrun;

Bitrun * shiftAndAdd(Bitrun * bitrun, bool bit);
void freeBitrun(Bitrun * bitrun);
Bitrun * createAndCopyBitrun(const Bitrun * bitrun);
Bitrun * createBitRun(void);
#endif // _BITRUN_