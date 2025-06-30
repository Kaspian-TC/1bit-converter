#include "1bit_types.h"
int getOneImageByteLength(OneImage *omg){
  return (int)ceil((float)(omg->sx* omg->sy)/8);
}
Pixel getPixel(Image *img, int pixIdx) {
  return img->data[pixIdx];
}
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
void freeImage(Image *im) {
  if (im) free(im->data);
  if (im->filename != NULL) free(im->filename);
  free(im);
}
