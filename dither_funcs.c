#include <time.h>
#include "dither_funcs.h"
#include "data_types/1bit_types.h"
#include <omp.h>
#define _INDEX(X,Y,W) ((X) + (Y) * (W))



/* vvvvvvvDITHER ALGORITHMSvvvvvvv */
void imgThreshholdMapDither(Image * img,int * threshhold_map,int x_size,int y_size,int threshhold_range){
//	#pragma omp parallel for schedule(dynamic,32)
	for(int y = 0; y< img->sy ;y+=y_size){for(int x = 0; x< img->sx ;x+=x_size){
		for(int i = 0; i<x_size;i++){for(int j = 0; j<y_size;j++){
			float threshhold = threshhold_map[_INDEX(i,j,x_size)] + 0.5; //0.5 is a normalization constant
			if(x+i < img->sx && y+j < img->sy){ // Check makes sure it doesn't go out of bounds
				Pixel current_pixel = getPixel(img,_INDEX(x+i,y+j,img->sx));
				Pixel new_Pixel;
				new_Pixel.R = (current_pixel.R*threshhold_range >= threshhold*255 ? 255 : 0);
				new_Pixel.G = (current_pixel.G*threshhold_range >= threshhold*255 ? 255 : 0);
				new_Pixel.B = (current_pixel.B*threshhold_range >= threshhold*255 ? 255 : 0);
				img->data[_INDEX(x+i,y+j,img->sx)] = new_Pixel;
			}
		}}
	}}
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
// makes sure that quantized image doesn't go out of bounds
static uint8_t correctError(float input_val){
	if(input_val>255){
		return 255;
	}
	else if(input_val<0){
		return 0;
	}
	return (uint8_t)input_val;
}
static void imgErrorDitherHelper(Pixel * pixels,int x,int y,float quant_err_R,float quant_err_G,float quant_err_B,float debt,int width,int height){ // needs to be rewritten a bit but gets the point accross 
	if(x<width && x>=0 && y<height){
		int index = _INDEX(x,y,width);
		Pixel c = pixels[index];
		float r = (float)c.R;
		float g = (float)c.G;
		float b = (float)c.B;
		r += quant_err_R * debt;
		g += quant_err_G * debt;
		b += quant_err_B * debt;
		c.R= correctError(r);
		c.G= correctError(g);
		c.B= correctError(b);
		pixels[index] = c;
	}
	return;
}

void imgErrorDither(Image * img, int colour_count,float *error_kernel,int *location_positions,int kernelSize){
	for(int y = 0; y< img->sy ;y++){for(int x = 0; x< img->sx ;x++){	
		Pixel oldpixel = img->data[_INDEX(x,y,img->sx)]; //oldpixel := pixels[x][y]
		float old_r = oldpixel.R;
		float old_g = oldpixel.G;
		float old_b = oldpixel.B;
		
		Pixel newpixel; //newpixel�:= find_closest_palette_color(oldpixel)
		newpixel.R = (uint8_t)round(colour_count * old_r / 255) * (255/colour_count);
		newpixel.G = (uint8_t)round(colour_count * old_g / 255) * (255/colour_count);
		newpixel.B = (uint8_t)round(colour_count * old_b / 255) * (255/colour_count);
		
		img->data[_INDEX(x,y,img->sx)] = newpixel; //pixels[x][y]�:= newpixel
		
		float quant_err_R = old_r-newpixel.R; //quant_error�:= oldpixel - newpixel
		float quant_err_G = old_g-newpixel.G;
		float quant_err_B = old_b-newpixel.B;
		
		for(int i = 0;i<kernelSize;i++){
			imgErrorDitherHelper(img->data,
							x+location_positions[2*i],
							y+location_positions[2*i+1],
							quant_err_R,
							quant_err_G,
							quant_err_B,
							error_kernel[i],
							img->sx,
							img->sy);
		}
	}}
	return;
}
void ditherFloydSteinberg(Image * img, int factor){
	float error_kernel[4] = {7.0/16,3.0/16,5.0/16,1.0/16};
	int location_positions[4][2] = 
	{
		{1,0},{-1,1},{0,1},{1,1}
	};
	imgErrorDither(img,factor,error_kernel,location_positions[0],4);
	return;
}
void ditherAtkinson(Image * img, int factor){
	float error_kernel[6] = {1.0/8,1.0/8,1.0/8,1.0/8,1.0/8,1.0/8};
	int location_positions[6][2] = 
	{
		{1,0},{2,0},{-1,1},{0,1},{1,1},{0,2}
	};
	imgErrorDither(img,factor,error_kernel,location_positions[0],6);
	return;
}
void ditherJarvisJudiceNinke(Image * img, int factor){
	float error_kernel[12] = {7,5,3,5,7,5,3,1,3,5,3,1};
	for(int i = 0; i<12;i++)
		error_kernel[i] /= 48.0;

	int location_positions[12][2] = 
	{
		{1,0},{2,0},
		{-2,1},{-1,1},{0,1},{1,1},{2,1},
		{-2,2},{-1,2},{0,2},{1,2},{2,2}
	};
	imgErrorDither(img,factor,error_kernel,location_positions[0],12);
	return;
}
void randomDither(Image *img){
	srand(time(NULL));
	for(int y = 0; y< img->sy ;y++)	{
		for(int x = 0; x< img->sx ;x++){
			Pixel current_pixel = getPixel(img,_INDEX(x,y,img->sx));
			Pixel new_Pixel;
			int random_number = rand() % 256;
			new_Pixel.R = (current_pixel.R >= random_number ? 255 : 0);
			new_Pixel.G = (current_pixel.G >= random_number ? 255 : 0);
			new_Pixel.B = (current_pixel.B >= random_number ? 255 : 0);
			img->data[_INDEX(x,y,img->sx)] = new_Pixel;
		}
	}
}
/* ^^^^^^^DITHER ALGORITHMS^^^^^^^ */
