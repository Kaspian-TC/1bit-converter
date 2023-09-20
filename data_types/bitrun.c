#include "bitrun.h"
#include <stddef.h>
#include <stdint.h> 
#include <stdbool.h>
#include <stdlib.h>
#define BITRUN_TYPE uint64_t

static const int run_type_size = sizeof(BITRUN_TYPE)*8;
typedef struct bitRun{
	BITRUN_TYPE * run;
	int length;
} Bitrun;
/**
 * @brief Return the bit at the index of the src. Index refers to the bit index
 * @param src 
 * @param index 
 * @return 0 or 1
 */
static uint8_t readBit(const BITRUN_TYPE src,const int index){
	BITRUN_TYPE bit = 1; 
	bit = bit<< ((run_type_size-1) - (index%(run_type_size)));
	if((src & bit) == 0) //there is a zero in that position
	{
		return 0;
	}
	else{
		return 1;
	}
}
static uint8_t changeBit(uint8_t byte,int index,bool val){//returns the changed byte 
	//index should only be from 0-7
	uint8_t bit = 1;
	bit = bit<< (7 - index);
	if((byte & bit) == 0){//there is not a 1 in that position
		if(val){//add a 1
			byte = byte + bit;
		}
	}
	else{//there is a 1 in that postion (not used in this program)
		if(!val){//remove a 1
			byte = byte - bit;
		}
	}
	return byte;
}
static void assignBit(uint8_t* dest, long index,bool val){//bit should only be 1 or 0
    uint8_t current_byte = dest[(index)/sizeof(uint8_t)];
    dest[(index)/sizeof(uint8_t)] = changeBit(current_byte,(index)%sizeof(uint8_t),val);
    return;
}
/**
 * @brief Return the bit at the index of the run in bitrun. Index refers to the 
 * bit index in the run array
 * @param bitrun 
 * @param index 
 * @return 0 or 1 or -1 if error
 */
static uint8_t readBitrunBit(const Bitrun * bitrun,const int index){
    if(bitrun == NULL){
        return -1;
    }
    if(index >= (bitrun->length * run_type_size)){
        return -1;
    }
    return readBit(bitrun->run[index/run_type_size],index%run_type_size);
}
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

    static const BITRUN_TYPE most_sig_bit = ((BITRUN_TYPE) 1) << (sizeof(BITRUN_TYPE)*8-1);
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
/**
 * @brief Assign bitrun to dest starting at dest_start. dest_start should be the
 * index of the first bit in dest that will be assigned to the first bit in
 * bitrun
 * @param bitrun 
 * @param dest 
 * @param dest_start 
 * @return updated value of dest_start
 */
long assignBitrunToMemory(const Bitrun * bitrun,uint8_t * dest,long dest_start){
    if(bitrun == NULL){
        return -1;
    }
    long starting_index = 0;
    while(readBitrunBit(bitrun,starting_index) == 0 && 
                        starting_index < bitrun->length*run_type_size){
        starting_index++;
    }
    starting_index++; // so it starts after the first 1
    const int bitrun_bit_length = bitrun->length*run_type_size-starting_index+1;

    for(long i = dest_start;i<dest_start+(bitrun_bit_length);i++){
        const int bitrun_index = i-dest_start+starting_index;
        assignBit(dest,i,readBitrunBit(bitrun,bitrun_index));
    }

    return dest_start+bitrun_bit_length;
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