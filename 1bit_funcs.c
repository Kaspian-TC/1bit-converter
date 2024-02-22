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

static uint8_t correctError(float input_val){//input value
	if(input_val>255){
		return 255;
	}
	else if(input_val<0){
		return 0;
	}
	return (uint8_t)input_val;
}
static int pixelSum(Pixel current_pixel){
	return (int)(current_pixel.R + current_pixel.G + current_pixel.B);
}
static double avgRGB(Pixel current_pixel){
	return (double)pixelSum(current_pixel)/3;
}
void imgThreshholdMapDither(Image * img,int * threshhold_map,int x_size,int y_size,int threshhold_range){
	for(int y = 0; y< img->sy ;y+=y_size){
		for(int x = 0; x< img->sx ;x+=x_size){
			for(int i = 0; i<x_size;i++){
				for(int j = 0; j<y_size;j++){
					int threshhold = threshhold_map[i+j*x_size];
					if(x+i < img->sx && y+j < img->sy){
						Pixel currentPixel = getPixel(img,_INDEX(x+i,y+j,img->sx));
						if(threshhold_range*pixelSum(currentPixel)>(threshhold+0)*765){// +1 to focus on the darker images 
							currentPixel = (Pixel) {255,255,255};
						}
						else{
							currentPixel = (Pixel) {0,0,0};
						}
						img->data[_INDEX(x+i,y+j,img->sx)] = currentPixel;
					}
				}
			}
		}
	}
	return;
}
void imgBayerZero(Image * img){
	int bayerMatrix[2][2] = 
	{
		{0,2},
		{3,1}
	};
	imgThreshholdMapDither(img,bayerMatrix[0],2,2,4);
	return;
}
void imgBayerOne(Image * img){
	int bayerMatrix[4][4] = 
	{
		{0,8,2,10},
		{12,4,14,6},
		{3,11,1,9},
		{15,7,13,5}
	};
	imgThreshholdMapDither(img,bayerMatrix[0],4,4,16);
	return;
}
void imgBayerTwo(Image * img){
	int bayerMatrix[8][8] = 
	{
		{0,32,8,40,2,34,10,42},
		{48,16,56,24,50,18,58,26},
		{12,44,4,36,14,46,6,38},
		{60,28,52,20,62,30,54,22},
		{3,35,11,43,1,33,9,41},
		{51,19,59,27,49,17,57,25},
		{15,47,7,39,13,45,5,37},
		{63,31,55,23,61,29,53,21}
	};
	imgThreshholdMapDither(img,bayerMatrix[0],8,8,64);
	return;
}
static void imgDitherHelper(Pixel * pixels,int x,int y,float quant_err_R,float quant_err_G,float quant_err_B,float debt,int width,int height){ // needs to be rewritten a bit but gets the point accross 
	if(x<width && x>=0 && y<height){
		int index = _INDEX(x,y,width);
		Pixel c = pixels[index];
		float r = (float)c.R;
		float g = (float)c.G;
		float b = (float)c.B;
		r += quant_err_R * debt/16.0; // why are we dividing by 16?
		g += quant_err_G * debt/16.0;
		b += quant_err_B * debt/16.0;
		c.R= correctError(r);
		c.G= correctError(g);
		c.B= correctError(b);
		pixels[index] = c;
	}
	return;
}

void imgErrorDither(Image * img, int factor,int *error_kernel,int *location_positions,int kernelSize){
	for(int y = 0; y< img->sy ;y++){
		for(int x = 0; x< img->sx ;x++){
			
			Pixel oldpixel = img->data[_INDEX(x,y,img->sx)]; //oldpixel := pixels[x][y]
			float old_r = oldpixel.R;
			float old_g = oldpixel.G;
			float old_b = oldpixel.B;
			
			Pixel newpixel; //newpixel�:= find_closest_palette_color(oldpixel)
			newpixel.R = (uint8_t)round(factor * old_r / 255) * (255/factor);
			newpixel.G = (uint8_t)round(factor * old_g / 255) * (255/factor);
			newpixel.B = (uint8_t)round(factor * old_b / 255) * (255/factor);
			
			img->data[_INDEX(x,y,img->sx)] = newpixel; //pixels[x][y]�:= newpixel
			
			float quant_err_R = old_r-newpixel.R; //quant_error�:= oldpixel - newpixel
			float quant_err_G = old_g-newpixel.G;
			float quant_err_B = old_b-newpixel.B;
			
			for(int i = 0;i<kernelSize;i++){
				imgDitherHelper(img->data,
								x+location_positions[2*i],
								y+location_positions[2*i+1],
								quant_err_R,
								quant_err_G,
								quant_err_B,
								error_kernel[i],
								img->sx,
								img->sy);
			}
		}
	}
	return;
}
void ditherFloydSteinberg(Image * img, int factor){
	int error_kernel[4] = {7,3,5,1};
	int location_positions[4][2] = 
	{
		{1,0},{-1,1},{0,1},{1,1}
	};
	imgErrorDither(img,factor,error_kernel,location_positions[0],4);
	return;
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
