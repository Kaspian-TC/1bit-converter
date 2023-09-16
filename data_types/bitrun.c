#include "bitrun.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

Bitrun * shiftBitLeft(Bitrun * bitrun){
    if(bitrun == NULL){
        return NULL;
    }
    //simple bitshift if length is 1
    if(bitrun->length == 1){
        bitrun->run[0] = bitrun->run[0] << 1;
        return bitrun;
    }
    //case where run needs to be expanded
    const int last_bit = 1 << sizeof(BITRUN_TYPE)*8-1;
    if(bitrun->run[0] & last_bit){
        bitrun = createBitRun(bitrun);
    }
    
    bitrun->run[0] = bitrun->run[0] << 1;
    for(int i = 1;i<bitrun->length;i++){
        bitrun->run[i-1] = bitrun->run[i-1] | (bitrun->run[i] >> sizeof(BITRUN_TYPE)*8-1);
        bitrun->run[i] = bitrun->run[i] << 1;
    }
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
Bitrun * createBitRun(Bitrun * bitrun){
    if(bitrun == NULL){
        Bitrun * new_run = malloc(sizeof(Bitrun));
        new_run->length = 1;
        new_run->run = malloc(sizeof(BITRUN_TYPE));
        return new_run;
    }
    //case where run needs to be expanded
    bitrun->length++;
    bitrun->run = realloc(bitrun->run,sizeof(BITRUN_TYPE)*bitrun->length);
    for(int i = bitrun->length-1;i>0;i--){
        bitrun->run[i] = bitrun->run[i-1];
    }
    bitrun->run[0] = 0;
    return bitrun;
}