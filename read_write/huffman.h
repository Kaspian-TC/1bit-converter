#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <stdint.h>
/** 
 * 0 is the left leaf while 1 is the right leaf
*/

uint8_t* huffmanEncode(const uint8_t* data,int byte_length,long* size);
#endif // HUFFMAN_H