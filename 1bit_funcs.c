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
/* static void copyData(uint8_t *dest, const uint8_t *src,long length){
	for(int i = 0;i<length;i++){
		dest[i] = src[i];
	}
	return;
} */

static uint8_t correctError(float input_val){//input value
	if(input_val>255){
		return 255;
	}
	else if(input_val<0){
		return 0;
	}
	return (uint8_t)input_val;
}
static double avgRGB(Pixel current_pixel){
	return (double)(current_pixel.R + current_pixel.G + current_pixel.B)/3;
}
void imgThreshholdMapDither(Image * img,int * threshhold_map,int x_size,int y_size,int threshhold_range){
	for(int y = 0; y< img->sy ;y+=y_size){
		for(int x = 0; x< img->sx ;x+=x_size){
			
			for(int i = 0; i<x_size;i++){
				for(int j = 0; j<y_size;j++){
					int threshhold = threshhold_map[i+j*x_size];
					if(x+i != img->sx && y+j != img->sy){
						Pixel currentPixel = getPixel(img,_INDEX(x+i,y+j,img->sx));
						if(threshhold_range*avgRGB(currentPixel)>(threshhold+1)*255){// +1 to focus on the darker images 
							currentPixel.R = 255;
							currentPixel.G = 255;
							currentPixel.B = 255;
						}
						else{
							currentPixel.R = 0;
							currentPixel.G = 0;
							currentPixel.B = 0;
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
void imgBayerOne(Image * img){ //these are cool but I need to generalize this
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
static void imgDitherHelper(Pixel * pixels,int x,int y,float quant_err_R,float quant_err_G,float quant_err_B,float debt,int width,int height){ // needs to be rewritten a bit but gets the point accross 
	if(x<width && x>=0 && y<height){
		int index = _INDEX(x,y,width);
		Pixel c = pixels[index];
		float r = (float)c.R;
		float g = (float)c.G;
		float b = (float)c.B;
		r += quant_err_R * debt/16.0;
		g += quant_err_G * debt/16.0;
		b += quant_err_B * debt/16.0;
		c.R= correctError(r);
		c.G= correctError(g);
		c.B= correctError(b);
		pixels[index] = c;
	}
	return;
}

void imgDither(Image * img, int factor){
	for(int y = 0; y< img->sy ;y++){
		for(int x = 0; x< img->sx ;x++){
			
			Pixel oldpixel = img->data[_INDEX(x,y,img->sx)]; //oldpixel := pixels[x][y]
			float old_r = oldpixel.R;
			float old_g = oldpixel.G;
			float old_b = oldpixel.B;
			
			Pixel newpixel; //newpixel := find_closest_palette_color(oldpixel)
			newpixel.R = (uint8_t)round(factor * old_r / 255) * (255/factor);
			newpixel.G = (uint8_t)round(factor * old_g / 255) * (255/factor);
			newpixel.B = (uint8_t)round(factor * old_b / 255) * (255/factor);
			
			img->data[_INDEX(x,y,img->sx)] = newpixel; //pixels[x][y] := newpixel
			
			float quant_err_R = old_r-newpixel.R; //quant_error := oldpixel - newpixel
			float quant_err_G = old_g-newpixel.G;
			float quant_err_B = old_b-newpixel.B;
			
			imgDitherHelper(img->data,x+1,y,quant_err_R,quant_err_G,quant_err_B,7.0,img->sx,img->sy);// pixels[x + 1][y    ] := pixels[x + 1][y    ] + quant_error × 7 / 16
			imgDitherHelper(img->data,x-1,y+1,quant_err_R,quant_err_G,quant_err_B,3.0,img->sx,img->sy);// pixels[x - 1][y + 1] := pixels[x - 1][y + 1] + quant_error × 3 / 16
			imgDitherHelper(img->data,x,y+1,quant_err_R,quant_err_G,quant_err_B,5.0,img->sx,img->sy);// pixels[x    ][y + 1] := pixels[x    ][y + 1] + quant_error × 5 / 16
			imgDitherHelper(img->data,x+1,y+1,quant_err_R,quant_err_G,quant_err_B,1.0,img->sx,img->sy);// pixels[x + 1][y + 1] := pixels[x + 1][y + 1] + quant_error × 1 / 16
		}
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
			
			Pixel newpixel; //newpixel := find_closest_palette_color(oldpixel)
			newpixel.R = (uint8_t)round(factor * old_r / 255) * (255/factor);
			newpixel.G = (uint8_t)round(factor * old_g / 255) * (255/factor);
			newpixel.B = (uint8_t)round(factor * old_b / 255) * (255/factor);
			
			img->data[_INDEX(x,y,img->sx)] = newpixel; //pixels[x][y] := newpixel
			
			float quant_err_R = old_r-newpixel.R; //quant_error := oldpixel - newpixel
			float quant_err_G = old_g-newpixel.G;
			float quant_err_B = old_b-newpixel.B;
			
			for(int i = 0;i<kernelSize;i++){
				imgDitherHelper(img->data,x+location_positions[2*i],y+location_positions[2*i+1],quant_err_R,quant_err_G,quant_err_B,error_kernel[i],img->sx,img->sy);
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
static uint8_t calcAvgGrey(Image *img){
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
		for(long i = 0; i<imgSize;i++){
			// 1 is white, 0 is black
			if(avgRGB(getPixel(img,i)) >= avgLimit && avgLimit != 255){
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
		img->filename = malloc(nameLen+1);
		strncpy(img->filename,omg->filename,nameLen);
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

/* static uint8_t *runLengthEncode(const uint8_t *data,int x_size,int y_size,long *size){ //encodes the data with run length encoding
	//if there is negative compression, makes new_data identical to data
	//aaaabbbbcdddddee will become aa4bb4cdd5ee2 unless there is negative compression in which case it will exit with NULL
	int length = ceil((float)(x_size* y_size)/8);
	uint8_t *new_data = calloc((size_t)length, sizeof(uint8_t));
	if(data == NULL){
		return new_data;
	}
	if(length==1){//edge case
		new_data[0] = data[0];
	}
	long i = 1;
	uint8_t prev_byte;
	uint8_t current_byte= data[0];
	char is_run = 0; //0 false, 1 true
	uint8_t run_length;
	long new_i=0;
	while(i<length && new_i!=length-1){
		prev_byte = current_byte;
		current_byte = data[i];
		
		if(current_byte == prev_byte){
			if(is_run){
				if(run_length==255){ //max value, write to new_data
					//IF THE VALUE IS 255, THE NEXT 2 BYTEs IN new_data WILL BE of current_byte and the run will start at 1
					new_data[new_i++]=run_length;//assign 255
					new_data[new_i++]=current_byte;//assign two bytes
					new_data[new_i++]=current_byte;
					run_length=0; 
				}
				run_length++;
			}
			else{ // start run
				run_length = 2;
				is_run = 1;
				new_data[new_i++]=prev_byte;
				new_data[new_i++]=current_byte;//assign byte
			}
		}
		else{
			if(is_run){
				is_run = 0;
				new_data[new_i]=run_length; //assign run length
				new_i++;
			}
			else{ //normal assign
				new_data[new_i]=prev_byte;//assign byte
				new_i++;
			}
		}
		i++;
	}
	if(is_run){
		is_run = 0;
		new_data[new_i]=run_length; //assign run length
		new_i++;
	}
	// printf("%ld %ld\n",i,new_i);
	if(new_i>=length-1){
		copyData(new_data,data,length);
		*size = length;
		
		return new_data;
	}
	*size = new_i;
	return new_data;
}
void oneBitWrite(OneImage *omg, char *filename, char *type) {
  if (omg == NULL || !(strcmp(type,"")==0 || strcmp(type,".0")==0 || strcmp(type,".1")==0)){
	  fprintf(stderr, "oneBitWrite(): wrong type\n");
  }
    if (omg->data != NULL) {
      FILE *f = fopen(filename, "wb+");
      if (f == NULL) {
        fprintf(stderr, "Unable to open file %s.\n", filename);
        return;
      }
      fprintf(f, "1bit%s\n",type);
      fprintf(f, "%d %d\n", omg->sx, omg->sy);
	  if(strcmp(type,"")==0|| strcmp(type,".0")==0){
		fwrite(omg->data, (size_t)ceil((float)(omg->sx* omg->sy)/8), sizeof(uint8_t), f);
	  }
	  else if(strcmp(type,".1")==0){
		uint8_t *data;
		long size;
		size = 0;
		data = runLengthEncode(omg->data,omg->sx,omg->sy,&size);
		fwrite(data, (size_t)size, sizeof(uint8_t), f);
		if(size==ceil((float)(omg->sx* omg->sy)/8)){
			fprintf(stderr, "encoding resulted in negative compression, will now regularly write\n"); 
			rewind(f);
			fprintf(f, "1bit.0\n");
		}
		free(data);
	  }
      fclose(f);
      return;
    }
  fprintf(stderr, "oneBitWrite(): Specified image is empty. Nothing output\n");
  return;
}
void oneBitOutput(OneImage *omg, char *filename){ //keeps older compatability
	oneBitWrite(omg,filename,"");// makes the 1bit.0 file
	return;
}
*/