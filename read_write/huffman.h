#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
/** 
 * 0 is the left leaf while 1 is the right leaf
*/

uint8_t* huffmanEncode(const uint8_t* data,int byte_length,long* size);
size_t huffmanDecode(uint8_t *data,FILE *f,long length);
#endif // HUFFMAN_H
