#ifndef __1BIT_FUNCS_H__
#define __1BIT_FUNCS_H__
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// #include "imgutils.h"
#include "data_types/1bit_types.h"
#ifndef __1BIT_TYPES__
#define __1BIT_TYPES__
typedef struct {
  char *filename;         
  uint8_t *data; // Actual data
  int sx, sy;
} OneImage;
#endif

void freeOneImage(OneImage *om);
Image * imgGrayscale(Image * img);
OneImage* convertImgToOne(Image *img);
void averageColourImage(Image* img);
Image* convertOneToImg(OneImage* omg);
void oneBitOutput(OneImage *omg, char *filename);

#endif