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
void imgThreshholdMapDither(Image * img,int * threshhold_map,int x_size,int y_size,int threshhold_range);
void imgBayerZero(Image * img);
void imgBayerOne(Image * img);
void imgDither(Image * img, int factor);
void imgErrorDither(Image * img, int factor,int *errorKernel,int *locationPositions,int kernelSize);
void ditherFloydSteinberg(Image * img, int factor);
Image * imgGrayscale(Image * img);
OneImage* convertImgToOne(Image *img);
Image* convertOneToImg(OneImage* omg);
void oneBitOutput(OneImage *omg, char *filename);
Image *readImage(char *filename);
OneImage *read1bitimage(char *filename);
void oneBitWrite(OneImage *omg, char *filename, char *type);

#endif