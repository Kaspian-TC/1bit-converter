#include "1bit_funcs.h"
#define _INDEX(X,Y,W) ((X) + (Y) * (W))
/* This code is dedicated to Josh and Lee, but Lee helped more than Josh 
*/
void freeOneImage(OneImage *om) {
	if (om->filename != NULL) free(om->filename);
	if (om->data != NULL) free(om->data);
	free(om);
}
static uint8_t readBit(OneImage* omg,int index){
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
static uint8_t changeBit(uint8_t byte,int index,char val){//returns the changed byte 
	//index should only be from 0-7
	uint8_t bit = 1;
	bit = bit<< (7 - index);
	if((byte & bit) == 0){//there is not a 1 in that position
		if(val == 1){//add a 1
			byte = byte + bit;
		}
	}
	else{//there is a 1 in that postion (not used in this program)
		if(val == 0){//remove a 1
			byte = byte - bit;
		}
	}
	return byte;
}
static void assignBit(OneImage* omg, long index,char val){//bit should only be 1 or 0
	uint8_t current_byte = omg->data[(index)/8];
	omg->data[(index)/8] = changeBit(current_byte,(index)%8,val);
	return;
}


static int pixelSum(Pixel current_pixel){
	return (int)(current_pixel.R + current_pixel.G + current_pixel.B);
}
static double avgRGB(Pixel current_pixel){
	return (double)pixelSum(current_pixel)/3;
}

Image * imgGrayscale(Image * img){//returns grayscale of img
	Image *gray_img = newImage(img->sx,img->sy);
    gray_img->filename = malloc(strlen(img->filename));
	strcpy(gray_img->filename,img->filename);
	uint8_t avgColour;
	for(long i = 0; i<img->sx * img->sy ;i++){
		avgColour = avgRGB(getPixel(img,i));
		
		gray_img->data[i].R = avgColour;
		gray_img->data[i].G = avgColour;
		gray_img->data[i].B = avgColour;
	}
	return gray_img;
}
/* Returns the average brightness of the given image */
static uint8_t calcAvgGrey(Image *img){
	long average = 0;
	long imgSize = img->sx * img->sy;
	for(long i = 0; i<imgSize -1;i++){
		average += pixelSum(getPixel(img,i));
	}
	return (uint8_t)round((1.0*average)/(imgSize*3));
}
/* Converts type Image to OneImage, but assumes the data has been converted already */
OneImage* convertImgToOne(Image *img){
	if (img == NULL) {		
		fprintf(stderr, "Unable to allocate memory for Oneimage structure\n");
		return (NULL);
	}
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
	for(long i = 0; i<imgSize;i++){
		// 1 is white, 0 is black
		if(getPixel(img,i).R >= 0x80){
			assignBit(omg,i,1);
		}
	}
	return omg;
}
/* Finds the average brightness does a threshhold on each pixel.*/
void averageColourImage(Image* img){
	long imgSize = img->sx * img->sy;
	int avgLimit = calcAvgGrey(img);
	for(long i = 0; i<imgSize;i++){
		// 1 is white, 0 is black
		Pixel new_pixel;
		if(avgRGB(getPixel(img,i)) >= avgLimit && avgLimit != 255){
			new_pixel = (Pixel) {255,255,255};
		}
		else{
			new_pixel = (Pixel) {0,0,0};
		}
		img->data[i] = new_pixel;
	}
	return;
}
Image* convertOneToImg(OneImage* omg){
	if(omg != NULL) {
		Image *img = (Image *)calloc(1, sizeof(Image));
		int nameLen = strlen(omg->filename);
		img->filename = malloc(nameLen+1);
		strncpy(img->filename,omg->filename,nameLen); //TODO: make this more generalized
		img->filename[nameLen-4] = 'p';
		img->filename[nameLen-3] = 'p';
		img->filename[nameLen-2] = 'm';
		img->filename[nameLen-1] = '\0';
		img->data = calloc(omg->sx * omg->sy,sizeof(Pixel));
		img->sx = omg->sx;
		img->sy = omg->sy;
		long imgSize = img->sx * img->sy;
		for(long i = 0; i<imgSize;i++){
			if(readBit(omg,i) == 1){//if the colour is white
				img->data[i].R = 0xFF;
				img->data[i].G = 0xFF;
				img->data[i].B = 0xFF;
			}
			else{ // if the colour is black
				img->data[i].R = 0;
				img->data[i].G = 0;
				img->data[i].B = 0;
			}
		}
		return img;
	}
	exit(1);
	fprintf(stderr, "Unable to allocate memory for Image structure\n");
	return (NULL);
}
