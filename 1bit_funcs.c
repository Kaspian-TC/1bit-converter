#include "1bit_funcs.h"
void freeOneImage(OneImage *om) {
	if (om->filename != NULL) free(om->filename);
	if (om->data != NULL) free(om->data);
	free(om);
}
uint8_t readBit(OneImage* omg,int index){
	uint8_t bit = 1;
	bit = bit<< (7 - (index%8));
	if((omg->data[(index)/8] & bit) == 0) //there is a zero in that position
	{
		return 0;
	}
	else{
		return 1;
	}
}
uint8_t changeBit(uint8_t byte,int index,char val){//returns the changed byte 
	//index should only be from 0-7
	uint8_t bit = 1;
	bit = bit<< (7 - index);
	if((byte & bit) == 0){//there is not a 1 in that position
		if(val == 1){//add a 1
			byte = byte + bit;
		}
	}
	else{//there is a 1 in that postion
		if(val == 0){//remove a 1
			byte = byte - bit;
		}
	}
	return byte;
}
void assignBit(OneImage* omg, long index,char val){//bit should only be 1 or 0
	uint8_t current_byte = omg->data[(index)/8];
	omg->data[(index)/8] = changeBit(current_byte,(index)%8,val);
	return;
}
double avgRGB(Pixel current_pixel){
	return (double)(current_pixel.R + current_pixel.G + current_pixel.B)/3;
}
uint8_t calcAvgGrey(Image *img){
	double average = 0;
	long imgSize = img->sx * img->sy;
	for(long i = 0; i<imgSize -1;i++){
		average += avgRGB(getPixel(img,i))/imgSize;
	}
	return (uint8_t)round(average);
}
OneImage* convertImgToOne(Image *img){//converts type Image to OneImage
	if (img != NULL) {
		OneImage *omg = (OneImage *)calloc(1, sizeof(OneImage));
		int nameLen = strlen(img->filename);
		omg->filename = malloc(nameLen+2);
		strcpy(omg->filename,img->filename);
		omg->filename[nameLen-3] = '1';
		omg->filename[nameLen-2] = 'b';
		omg->filename[nameLen-1] = 'i';
		omg->filename[nameLen] = 't';
		omg->filename[nameLen+1] = '\0';
		omg->data = calloc((size_t)ceil((float)(img->sx * img->sy)/8), sizeof(uint8_t));
		omg->sx = img->sx;
		omg->sy = img->sy;
		long imgSize = img->sx * img->sy;
		int avgLimit = calcAvgGrey(img);
		for(long i = 0; i<imgSize -1;i++){
			// 1 is white, 0 is black
			if(avgRGB(getPixel(img,i)) <= avgLimit && avgLimit != 255){
				assignBit(omg,i,1);
			}
		}
		return omg;
	}
	fprintf(stderr, "Unable to allocate memory for Oneimage structure\n");
	return (NULL);
}
Image* convertOneToImg(OneImage* omg){
	if(omg != NULL) {
		Image *img = (Image *)calloc(1, sizeof(Image));
		int nameLen = strlen(omg->filename);
		img->filename = malloc(nameLen);
		strncpy(img->filename,omg->filename,nameLen-1);
		img->filename[nameLen-4] = 'p';
		img->filename[nameLen-3] = 'p';
		img->filename[nameLen-2] = 'm';
		img->filename[nameLen-1] = '\0';
		img->data = calloc(omg->sx * omg->sy,sizeof(Pixel));
		img->sx = omg->sx;
		img->sy = omg->sy;
		long imgSize = img->sx * img->sy;
		printf("%ld\n",imgSize);
		for(long i = 0; i<imgSize -1;i++){
			if(readBit(omg,i) == 0){//if the colour is black
				img->data[i].R = 255;
				img->data[i].G = 255;
				img->data[i].B = 255;
			}
		}
		return img;
	}
	fprintf(stderr, "Unable to allocate memory for Image structure\n");
	return (NULL);
}
void oneBitOutput(OneImage *omg, char *filename) {
  if (omg != NULL)
    if (omg->data != NULL) {
      FILE *f = fopen(filename, "wb+");
      if (f == NULL) {
        fprintf(stderr, "Unable to open file %s.\n", filename);
        return;
      }
      fprintf(f, "1bit\n");
      fprintf(f, "%d %d\n", omg->sx, omg->sy);
      fwrite(omg->data, (size_t)ceil((float)(omg->sx* omg->sy)/8), sizeof(uint8_t), f);
      fclose(f);
      return;
    }
  fprintf(stderr, "imageOutput(): Specified image is empty. Nothing output\n");
  return;
}
OneImage *read1bitimage(char *filename) {
	int x;
	char *y;
	OneImage *omg = (OneImage *)calloc(1, sizeof(OneImage));
	if (omg != NULL) {
		char buffer[1024];

		FILE *f = fopen(filename, "rb+");
		if (f == NULL) {
		fprintf(stderr,"Unable to open file %s. Check the path.\n",filename);
		exit(1);
		}
		char *shortend_filename = basename(filename); 
		omg->filename = malloc(strlen(shortend_filename));
		strcpy(omg->filename,shortend_filename);

		y = fgets(buffer, 1024, f);//Read and check header
		if (strcmp(buffer, "1bit\n") != 0) {
			fprintf(stderr, "%s: Wrong file format, not a .1bit file.\n", filename);
			exit(1);
		}
		do { y = fgets(buffer, 1024, f); //Skip over comments
			if(y == NULL){
				fprintf(stderr, "Error reading file\n");
				exit(1);
			}
		} while (buffer[0] == '#');

		sscanf(buffer, "%d %d\n", &omg->sx, &omg->sy); //Read file size

		omg->data = calloc((size_t)ceil((float)(omg->sx* omg->sy)/8), sizeof(uint8_t));
		if (omg->data == NULL) {
			fprintf(stderr, "Out of memory allocating space for image\n");
			exit(1);
		}

		x = fread(omg->data, sizeof(uint8_t), (size_t)ceil((float)(omg->sx* omg->sy)/8), f);
		if(x<(omg->sx * omg->sy)/8){
			fprintf(stderr, "expected read value %d while actually read %d\n",(omg->sx * omg->sy)/8,x);
			exit(1);
		}
		fclose(f);

		return omg;
	}

	fprintf(stderr, "Unable to allocate memory for image structure\n");
	return (NULL);
}