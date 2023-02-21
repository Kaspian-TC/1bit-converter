#ifndef __1BIT_TYPES__
#define __1BIT_TYPES__
#ifndef __IMGUTILS_H__
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
#endif
typedef struct {
  char *filename;         
  uint8_t *data; // Actual data
  int sx, sy;
} OneImage;

#endif