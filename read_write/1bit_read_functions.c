// #include "1bit_funcs.h"
#include "1bit_read_functions.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNM
#include "stb/stb_image.h" //this should only be included here since this is the READ file
#endif
 // TODO: credit nothings/stb 
static char *basename(char *path) {
  int l = strlen(path)-1;
  for (int i = l; i >= 0; i--) {
    if (path[i] == '/' || path[i] == '\\') {
      return &path[i+1];
    }
  }
  return path;
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
			fprintf(stderr,"Unable to open file %s in read1bitimage. Check the path.\n",filename);
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
Image *readImage(char *filename){ // returns the Image type 

  Image *img = (Image *)calloc(1, sizeof(Image));
  if (img != NULL) {
    
    img->data = NULL;
	
	char *shortend_filename = basename(filename); //takes the 
    img->filename = malloc(strlen(shortend_filename));
	strcpy(img->filename,shortend_filename);
    
	int width, height, bpp, ok;
	
    ok = stbi_info(filename, &width, &height, &bpp);
	
	if(!ok){
		fprintf(stderr, "File type not supported\n");
		free(img->filename);
		free(img);
		exit(1);
	}
	// Read file size
	img->sx = width;
	img->sy = height;
	if(bpp>3){
		fprintf(stderr, "Transparency layer was lost\n");
	}
    uint8_t* rgb_image_data = stbi_load(filename, &width, &height, &bpp, 3); // Read file data
	img->data = (Pixel *) rgb_image_data;
    
    if (rgb_image_data == NULL) {
		fprintf(stderr, "Out of memory allocating space for image\n");
		free(img->filename);
		free(img);
		exit(1);
    }
    return img;
  }

  fprintf(stderr, "Unable to allocate memory for image structure\n");
  return (NULL);
}