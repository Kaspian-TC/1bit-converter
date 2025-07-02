#ifndef __DITHER_FUNCS_H__
#define __DITHER_FUNCS_H__
#include "data_types/1bit_types.h"

void imgThreshholdMapDither(Image * img,int * threshhold_map,int x_size,int y_size,int threshhold_range);
void imgBayerZero(Image * img);
void imgBayerOne(Image * img);
void imgBayerTwo(Image * img);
void imgErrorDither(Image * img, int factor,float *errorKernel,int *locationPositions,int kernelSize);
void ditherFloydSteinberg(Image * img, int factor);
void ditherAtkinson(Image * img, int factor);
void ditherJarvisJudiceNinke(Image * img, int factor);
#endif