/**
 * You shouldn't need to change any of the code here. You will
 * NOT be submitting this file as part of your assignment. You
 * might want to look at the getPixel() function. Do not
 * change anything here as your code will be run with the file as
 * provided in the starter code.
 *
 * CSCB63 Winter 2022 - Coding Assignment 2
 * (c) Mustafa Quraish
 */

#include "imgutils.h"

// Given (a pointer to) an image, and the pixel index, returns 
// the the corresponding pixel. Assumes that pixel index is valid. Note that 
// the return value is *not* a pointer.
Pixel getPixel(Image *img, int pixIdx) {
  return img->data[pixIdx];
}

// Generates a new image. You don't need to look a this code if
// you don't want to. It's not relevant to what you have to implement.
Image *newImage(int sx, int sy) {
  Image *img;

  img = (Image *)calloc(1, sizeof(Image));
  if (img != NULL) {
    img->sx = sx;
    img->sy = sy;
    img->data = calloc(sx * sy, sizeof(Pixel));
    if (img->data != NULL) 
      return (img);
  }
  fprintf(stderr, "Unable to allocate memory for new image\n");
  return (NULL);
}

char *basename(char *path) {
  int l = strlen(path)-1;
  for (int i = l; i >= 0; i--) {
    if (path[i] == '/' || path[i] == '\\') {
      return &path[i+1];
    }
  }
  return path;
}

// Read in an image from the given filename
Image *readPPMimage(char *filename) {

  int x;
  char *y;

  Image *img = (Image *)calloc(1, sizeof(Image));
  if (img != NULL) {
    char buffer[1024];
    
    img->data = NULL;
    FILE *f = fopen(filename, "rb+");
    if (f == NULL) {
      fprintf(stderr,"Unable to open file %s. Check the path.\n",filename);
      exit(1);
    }
	char *shortend_filename = basename(filename); 
    img->filename = malloc(strlen(shortend_filename));
	strcpy(img->filename,shortend_filename);
    
    // Read and check header
    y = fgets(buffer, 1024, f);
    if (strcmp(buffer, "P6\n") != 0) {
      fprintf(stderr, "%s: Wrong file format, not a .ppm file.\n", filename);
      exit(1);
    }

    // Skip over comments
    do { y = fgets(buffer, 1024, f); } while (buffer[0] == '#');

    // Read file size
    sscanf(buffer, "%d %d\n", &img->sx, &img->sy);

    // Read the remaining header line
    y = fgets(buffer, 1024, f);
      
    img->data = calloc(img->sx * img->sy, sizeof(Pixel));
    if (img->data == NULL) {
      fprintf(stderr, "Out of memory allocating space for image\n");
      exit(1);
    }

    x = fread(img->data, sizeof(Pixel), img->sx * img->sy, f);
    fclose(f);

    return img;
  }

  fprintf(stderr, "Unable to allocate memory for image structure\n");
  return (NULL);
}

// Output an image at the given filename
void imageOutput(Image *img, char *filename) {
  if (img != NULL)
    if (img->data != NULL) {
      FILE *f = fopen(filename, "wb+");
      if (f == NULL) {
        fprintf(stderr, "Unable to open file %s.\n", filename);
        return;
      }
      fprintf(f, "P6\n");
      fprintf(f, "%d %d\n", img->sx, img->sy);
      fprintf(f, "255\n");
      fwrite(img->data, img->sx * img->sy, sizeof(Pixel), f);
      fclose(f);
      return;
    }
  fprintf(stderr, "imageOutput(): Specified image is empty. Nothing output\n");
  return;
}

// Output the path onto the image. Colour changes from light
// green to dark green based on when the pixel along the
// path was visited
void outputPath(int path[], Image *img) {
  
  // Get length of path:
  int n = 0;
  while (path[n] >= 0) { n++; }
  
  Image *pathIm = newImage(img->sx, img->sy);
  memcpy(pathIm->data, img->data, sizeof(Pixel) * img->sx * img->sy);

  double l = 120.0 / n;
  for (int p = 0; p < n; p++) {
    Pixel col = { 0, 255 - (p * l), 0 };
    pathIm->data[path[p]] = col;
  }
  
  char outputName[1024];
  sprintf(outputName, "Path-%s", img->filename);
  imageOutput(pathIm, outputName);
}

void freeImage(Image *im) {
  if (im) free(im->data);
  free(im);
}
