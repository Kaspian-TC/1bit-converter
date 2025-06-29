#ifndef __1BIT_READ_FUNCS_H__
#define __1BIT_READ_FUNCS_H__
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../data_types/1bit_types.h"
// #include "../imgutils.h"//TODO: remove later
#ifndef __1BIT_FUNCS_H__
/* typedef struct {
  char *filename;
  uint8_t *data;
  int sx, sy;
} OneImage;
 */
#endif
Image *readImage(char *filename);
OneImage *read1bitimage(char *filename);
#endif