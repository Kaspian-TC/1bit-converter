#ifndef __1BIT_WRITE_FUNCS_H__
#define __1BIT_WRITE_FUNCS_H__
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../1bit_types.h"
// #include "../imgutils.h"//TODO: remove later
#ifndef __1BIT_FUNCS_H__
/* typedef struct {
  char *filename;
  uint8_t *data;
  int sx, sy;
} OneImage; */
#endif
void writeImage(Image *img, char *filename);
void oneBitOutput(OneImage *omg, char *filename);
void oneBitWrite(OneImage *omg, char *filename, char *type);
// static uint8_t *runLengthEncode(const uint8_t *data,int x_size,int y_size,long *size);
#endif