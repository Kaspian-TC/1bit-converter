#include "bitrun.h"
#include <stddef.h>
#include <stdint.h> 
#include <stdbool.h>
#include <stdlib.h>
#define BITRUN_TYPE uint64_t
typedef struct bitRun{
	BITRUN_TYPE * run;
	int length;
} Bitrun;
static Bitrun * extendBitRun(Bitrun * bitrun,int extend_by){
    bitrun->length+=extend_by;
    bitrun->run = realloc(bitrun->run,sizeof(BITRUN_TYPE)*bitrun->length);
    for(int i = bitrun->length-1;i>0;i--){
        bitrun->run[i] = bitrun->run[i-1];
    }
    bitrun->run[0] = 0;
    return bitrun;
}
static Bitrun * shiftBitLeft(Bitrun * bitrun){
    if(bitrun == NULL){
        return NULL;
    }

    static const BITRUN_TYPE most_sig_bit = 
                ((BITRUN_TYPE) 1) << (sizeof(BITRUN_TYPE)*8-1);
    //case where run needs to be expanded
    bool is_most_sig = bitrun->run[0] & most_sig_bit;
    
    bitrun->run[0] = bitrun->run[0] << 1;
    for(int i = 1;i<bitrun->length;i++){
        bitrun->run[i-1] = bitrun->run[i-1] | ((bitrun->run[i] & most_sig_bit) != 0);
        bitrun->run[i] = bitrun->run[i] << 1;
    }

    if(is_most_sig) {
        bitrun = extendBitRun(bitrun,1);
        bitrun->run[0] = bitrun->run[0] | 1;
    }

    return bitrun;
}

Bitrun * shiftAndAdd(Bitrun * bitrun,bool bit){
    if(bitrun == NULL){
        return NULL;
    }
    Bitrun * new_run = shiftBitLeft(bitrun);
    
    if(bit){
        new_run->run[new_run->length-1] = new_run->run[new_run->length-1] | 1;
    }
    return new_run;
}

void freeBitrun(Bitrun * bitrun){
    free(bitrun->run);
    free(bitrun);
    return;
}
Bitrun * createAndCopyBitrun(const Bitrun * bitrun){
    Bitrun * new_bitrun = createBitRun();
    new_bitrun->length = bitrun->length;
    new_bitrun = extendBitRun(new_bitrun,bitrun->length-1);
    for(int i = 0;i<bitrun->length;i++){
        new_bitrun->run[i] = bitrun->run[i];
    }
    return new_bitrun;
}
Bitrun * createBitRun(void){
    Bitrun * new_run = malloc(sizeof(Bitrun));
    new_run->length = 1;
    new_run->run = malloc(sizeof(BITRUN_TYPE));
    return new_run;
}