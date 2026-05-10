#ifndef __1BIT_WRITE_FUNCS_H__
#define __1BIT_WRITE_FUNCS_H__
#include <stdint.h>
#include <stdbool.h>
#include "../data_types/1bit_types.h"
#ifndef __1BIT_FUNCS_H__
#endif
void writeImage(Image *img, char *filename);
void oneBitOutput(OneImage *omg, char *filename);
void oneBitWrite(OneImage *omg, char *filename, char *type);
#endif
