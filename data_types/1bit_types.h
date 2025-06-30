#ifndef __1BIT_TYPES__
#define __1BIT_TYPES__
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  uint8_t R;
  uint8_t G;
  uint8_t B;
} Pixel;
typedef struct {
  char *filename;         
  Pixel *data; // Actual pixel data
  int sx, sy;
} Image;
typedef struct {
  char *filename;         
  uint8_t *data; // Actual data
  int sx, sy;
} OneImage;
int getOneImageByteLength(OneImage *omg);
Pixel getPixel(Image *im, int pixIdx);
Image *newImage(int sx, int sy);
char *basename(char *path);
void freeImage(Image *im);

#endif