#include "1bit_funcs.h"

#define _INDEX(X,Y,W) ((X) + (Y) * (W))
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
/* This code is dedicated to Josh and Lee, but Lee helped more than Josh 
*/
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
static void copyData(uint8_t *dest, const uint8_t *src,long length){
	for(int i = 0;i<length;i++){
		dest[i] = src[i];
	}
	return;
}
static double avgRGB(Pixel current_pixel){
	return (double)(current_pixel.R + current_pixel.G + current_pixel.B)/3;
}
static uint8_t calcAvgGrey(Image *img){
	double average = 0;
	long imgSize = img->sx * img->sy;
	for(long i = 0; i<imgSize -1;i++){
		average += avgRGB(getPixel(img,i))/imgSize;
	}
	return (uint8_t)round(average);
}
uint8_t correctError(float input_val){//input value
	if(input_val>255){
		return 255;
	}
	else if(input_val<0){
		return 0;
	}
	return (uint8_t)input_val;
}

void imgDither(Image * img, int factor){
	Pixel current_pixel;
	for(int y = 0; y< img->sy ;y++){
		for(int x = 0; x< img->sx ;x++){
			current_pixel = img->data[_INDEX(x,y,img->sx)];
			float oldR = current_pixel.R;
			float oldG = current_pixel.G;
			float oldB = current_pixel.B;
			
			uint8_t newR = (uint8_t)round(factor * oldR / 255) * (255/factor);
			uint8_t newG = (uint8_t)round(factor * oldG / 255) * (255/factor);
			uint8_t newB = (uint8_t)round(factor * oldB / 255) * (255/factor);
			Pixel new_pixel;
			
			new_pixel.R = newR;
			new_pixel.G = newG;
			new_pixel.B = newB;
			
			img->data[_INDEX(x,y,img->sx)] = new_pixel;
			
			float errR = oldR-newR;
			float errG = oldG-newG;
			float errB = oldB-newB;
			
			int index;
			Pixel c;
			float r;
			float g;
			float b;
			float debt = 5;
			// printf("%f ",debt);
			/* for(int i=0;i<4;i++){
				// printf("%f ",debt);
				index = _INDEX(x+(i !=0 ? i-2 : 1),y+(i>0),img->sx); // for x value i !=0 ? i-2 : 1 ; for y i>0 ? 1 : 0
				// printf("%d, %d |",x+(i !=0 ? i-2 : 1),y+(i>0));
				c = img->data[index];
				r = (float)c.R;
				g = (float)c.G;
				b = (float)c.B;
				if(i % 2){
					debt -= 4.0;
				}
				else{
					debt += 2.0;
				}
				r += errR * debt/16.0;
				g += errG * debt/16.0;
				b += errB * debt/16.0;
				// printf("%f ",debt);
				c.R= correctError(r);
				c.G= correctError(g);
				c.B= correctError(b);
				img->data[index] = c;
			} */
			// printf("\n");
			if(x+1<img->sx){
				index = _INDEX(x+1,y,img->sx);
				c = img->data[index];
				r = (float)c.R;
				g = (float)c.G;
				b = (float)c.B;
				r += errR * 7.0/16.0;
				g += errG * 7.0/16.0;
				b += errB * 7.0/16.0;
				c.R= correctError(r);
				c.G= correctError(g);
				c.B= correctError(b);
				img->data[index] = c;
			}
			if(x-1>=0 && y+1<img->sy){
				index = _INDEX(x-1,y+1,img->sx);
				c = img->data[index];
				r = (float)c.R;
				g = (float)c.G;
				b = (float)c.B;
				r += errR * 3.0/16.0;
				g += errG * 3.0/16.0;
				b += errB * 3.0/16.0;
				c.R= correctError(r);
				c.G= correctError(g);
				c.B= correctError(b);
				img->data[index] = c;
			}
			if(y+1<img->sy){
				index = _INDEX(x,y+1,img->sx);
				c = img->data[index];
				r = (float)c.R;
				g = (float)c.G;
				b = (float)c.B;
				r += errR * 5.0/16.0;
				g += errG * 5.0/16.0;
				b += errB * 5.0/16.0;
				c.R= correctError(r);
				c.G= correctError(g);
				c.B= correctError(b);
				img->data[index] = c;
			}
			if(x+1<img->sx && y+1<img->sy){
				index = _INDEX(x+1,y+1,img->sx);
				c = img->data[index];
				r = (float)c.R;
				g = (float)c.G;
				b = (float)c.B;
				r += errR * 1.0/16.0;
				g += errG * 1.0/16.0;
				b += errB * 1.0/16.0;
				c.R= correctError(r);
				c.G= correctError(g);
				c.B= correctError(b);
				img->data[index] = c;
			}
			// img->data[_INDEX(x+1,y,img->sx)] = c;
			// img->data[_INDEX(x-1,y+1,img->sx)] = c;
			// img->data[_INDEX(x,y+1,img->sx)] = c;
			// img->data[_INDEX(x+1,y+1,img->sx)] = c;
			
		}
	}
	
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
		// printf("%d, %d, %d\n",img->data[i].R,img->data[i].G,img->data[i].B);
	}
	return gray_img;
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

static uint8_t *runLengthEncode(const uint8_t *data,int x_size,int y_size,long *size){ //encodes the data with run length encoding
	//if there is negative compression, makes new_data identical to data
	/*aaaabbbbcdddddee will become aa4bb4cdd5ee2 unless there is negative compression in which case it will exit with NULL*/
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
					run_length=0; // TODO large files seem to add extra on the end
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
	if(new_i==length-1){
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
		if(data==omg->data){
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
static char assignValues(uint8_t *data,const long pos_1,const int amount, const long length, const uint8_t value){//helper function for runLengthDecode
	if(pos_1+amount>=length){
		return 0;// fail
	}
	for(long i = 0;i<amount && i+pos_1<length;i++){
		data[i+pos_1] = value;
	}
	return 1;
}

static long runLengthDecode(uint8_t *data,FILE *f,long length){ //f must start at the 
	if(length ==0){
		return 0;
	}
	else if(length==1){
		data[0] = (uint8_t)fgetc(f);
		return 1;
	}
	uint8_t current_byte;
	uint8_t prev_byte;
	long d_index = 0;
	long f_index = 0;
	current_byte = (uint8_t)fgetc(f);
	f_index++;
	do{ //stops loop when at eof
		prev_byte = current_byte;
		current_byte = (uint8_t)fgetc(f);
		f_index++;
		if(current_byte == prev_byte){ //there is a run
			uint8_t run_length = (uint8_t)fgetc(f);
			f_index++;
			assignValues(data,d_index,run_length,length,current_byte);
			d_index+=run_length;
			current_byte =(uint8_t)fgetc(f);
			f_index++;
		}
		else{
			data[d_index] = prev_byte;
			d_index++; 
		}
	}while(!feof(f) && d_index<length);
	return d_index;
}
OneImage *read1bitimage(char *filename) {
	long x;
	char *y;
	OneImage *omg = (OneImage *)calloc(1, sizeof(OneImage));
	if (omg != NULL) {
		char buffer[1024];
		char type;
		FILE *f = fopen(filename, "rb+");
		if (f == NULL) {
			fprintf(stderr,"Unable to open file %s. Check the path.\n",filename);
			exit(1);
		}
		char *shortend_filename = basename(filename); 
		omg->filename = malloc(strlen(shortend_filename)+1);
		strcpy(omg->filename,shortend_filename);

		y = fgets(buffer, 1024, f);//Read and check header
		if (strcmp(buffer, "1bit\n") == 0 || strcmp(buffer, "1bit.0\n") == 0 ) {
			type = 0;
		}
		else if (strcmp(buffer, "1bit.1\n") == 0) {
			type = 1;
		}
		else{
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
			fprintf(stderr, "Out of memory allocating space for image\n"); //calloc returns null when you cannot
			exit(1);
		}
		if(type == 0){
			x = fread(omg->data, sizeof(uint8_t), (size_t)ceil((float)(omg->sx* omg->sy)/8), f);
			if(x<(omg->sx * omg->sy)/8){
				fprintf(stderr, "expected read value %d while actually read %ld\n",(omg->sx * omg->sy)/8,x);
				exit(1);
			}
		}
		else if(type == 1){
			x = runLengthDecode(omg->data,f,ceil((float)(omg->sx* omg->sy)/8));
		}
		for(int i = 0;i<ceil((float)(omg->sx* omg->sy)/8);i++){
			// printf("%x ",omg->data[i]);
		}
		
		fclose(f);

		return omg;
	}

	fprintf(stderr, "Unable to allocate memory for image structure\n");
	return (NULL);
}
// OneImage * upgrade1bit(omg,,char *type){
	
// }