#include "1bit_types.h"
int getOneImageByteLength(OneImage *omg){
  return (int)ceil((float)(omg->sx* omg->sy)/8);
}