/*
 * You shouldn't need to change any of the code here. You will
 * NOT be submitting this file as part of your assignment. You
 * might want to look at the getColourAtPixel() function. Do not
 * change anything here as your code will be run with the file as
 * provided in the starter code.
 *
 * CSCB63 Winter 2022 - Coding Assignment 2
 * (c) Mustafa Quraish, Francisco Estrada
 *      (thanks to Paco for some of the code to handle .ppm files!)
 */

#ifndef __IMGUTILS_H__
#define __IMGUTILS_H__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*****************************************************************************/
/*    The type `uint8_t` stores an integer value between 0-255 inclusive     */
/*****************************************************************************/

typedef struct {
  uint8_t R;
  uint8_t G;
  uint8_t B;
} Pixel;

// Struct we will use for actually storing the image data. sx and sy store the 
// dimensions for the image. The image data is actually stored in `data`.
typedef struct {
  char *filename;         
  Pixel *data; // Actual pixel data
  int sx, sy;
} Image;

Pixel getPixel(Image *im, int pixIdx);
Image *newImage(int sx, int sy);
char *basename(char *path);
Image *readPPMimage(char *filename);
void imageOutput(Image *im, char *filename);
void outputPath(int path[], Image *img);
void freeImage(Image *im);

#endif // __IMGUTILS_H__
