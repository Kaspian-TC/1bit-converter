#ifndef __1BIT_FUNCS_H__
#define __1BIT_FUNCS_H__
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "imgutils.h"
typedef struct {
  char *filename;         
  uint8_t *data; // Actual data
  int sx, sy;
} OneImage;
void freeOneImage(OneImage *om);
OneImage* convertImgToOne(Image *img);
Image* convertOneToImg(OneImage* omg);
void oneBitOutput(OneImage *omg, char *filename);
OneImage *read1bitimage(char *filename);

#endif