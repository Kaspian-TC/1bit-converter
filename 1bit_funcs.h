#ifndef __1BIT_FUNCS_H__
#define __1BIT_FUNCS_H__
#include <stdint.h>
#include "data_types/1bit_types.h"

void freeOneImage(OneImage *om);
Image * imgGrayscale(Image * img);
OneImage* convertImgToOne(Image *img);
void averageColourImage(Image* img);
Image* convertOneToImg(OneImage* omg);
void oneBitOutput(OneImage *omg, char *filename);

#endif
