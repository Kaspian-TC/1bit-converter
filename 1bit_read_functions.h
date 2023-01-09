#ifndef __1BIT_READ_FUNCS_H__
#define __1BIT_READ_FUNCS_H__
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "imgutils.h"//TODO: remove later

typedef struct {
  char *filename;
  uint8_t *data;
  int sx, sy;
} OneImage;
Image *readImage(char *filename);
OneImage *read1bitimage(char *filename);
#endif