#include "../data_types/minheap.h"
#include <stdint.h>
/** 
 * 0 is the left leaf while 1 is the right leaf
*/

uint8_t* huffmanEncode(const uint8_t* data,int byte_length,long* size);