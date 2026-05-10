#ifndef __1BIT_TYPES__
#define __1BIT_TYPES__
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t R;
  uint8_t G;
  uint8_t B;
} Pixel;
typedef struct {
  Pixel *data; // Actual pixel data
  int sx, sy;
} Image;
typedef struct {
  uint8_t *data; // Actual data
  int sx, sy;
} OneImage;
size_t getOneImageByteLength(OneImage *omg);
Pixel getPixel(Image *im, int pixIdx);
Image *newImage(int sx, int sy);
void freeImage(Image *im);

#endif
