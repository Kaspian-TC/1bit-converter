#include <stdio.h>
#include "imgutils.h"
#include "1bit_funcs.h"
char * toUpper(char * s){ //changes the string to be uppercase only
	for(int i = 0; s[i]!='\0';i++){
		if(s[i]>='a' && s[i]<='z'){
			s[i] -= 32; //converts to upper case letter
		}
	}
	return s;
}
unsigned char fileIsValid(char *filename){// return 1 if valid, 0 otherwise
	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr,"Unable to open file %s. Check the path.\n",filename);
		return 0;
	}
	fclose(f);
	return 1;	
}
int main(int argc, char** argv){
	/* -r : read flag
	-c : create flag
	-u : upgrade flag
	-i : precides the two file paths
	-d : precides dithering type
	-co : colour flag for create, must be followed by two hexidecimals in form #XXXXXX
	*/
	char is_read = 0;
	char is_create = 0;
	char is_upgrade = 0;
	char is_dithered = 0;
	
	char * dither_type;
	char * input_file;
	char * output_file;
	char * file_version = NULL;
	for(int i = 1;i<argc;i++){
		if(strcmp(argv[i],"-r")==0){
			is_read=1;
		}
		else if(strcmp(argv[i],"-c")==0){
			is_create=1;
			file_version = argv[i+1];
		}
		else if(strcmp(argv[i],"-u")==0){
			is_upgrade = 1;
			file_version = argv[i+1];
		}
		else if(strcmp(argv[i],"-d")==0){
			is_dithered = 1;
			dither_type = argv[i+1];
		}
		else if(strcmp(argv[i],"-i")==0 && i+2<argc){
			input_file = argv[i+1];
			output_file = argv[i+2];
		}
	}
	
	if(is_read + is_create + is_upgrade != 1){
		fprintf(stderr,"must include -r,-c, or -u (but only one)\n");
		return 1;
	}
	if(!(fileIsValid(input_file))){
		fprintf(stderr,"input file path invalid or doesn't exist\n");
		return 1;
	}
	
	if(is_create){
		Image *img = readImage(input_file);
		OneImage *omg;
		if(is_dithered){
			Image * gray_img = imgGrayscale(img);
			freeImage(img);
			img = gray_img; //grayscale the image

			char upper_dither_type[strlen(dither_type)+1];
			strcpy(upper_dither_type,dither_type);
			toUpper(upper_dither_type);
			if(strcmp(upper_dither_type,"FLOYD-STEINBERG")==0){ //check what form of dithering it uses
				ditherFloydSteinberg(img,1);
				// imageOutput(gray_img,"TEST_FILE.ppm"); //something to do with the edges
				// omg = convertImgToOne(gray_img); // isn't working for some reason
				// freeImage(gray_img);
			}
			else if(strcmp(upper_dither_type,"BAYER-0")==0){
				imgBayerZero(img);
			}
			else{ // error, end program
				fprintf(stderr,"invalid dithering type specified\n");
				freeImage(img);
				return 1;
			}
		}
		omg = convertImgToOne(img); //always finds the average and threshhold dithers
		
		if(file_version != NULL){
			oneBitWrite(omg, output_file,file_version);
		}
		else{
			oneBitOutput(omg, output_file);
		}
		freeOneImage(omg);
		freeImage(img);
	}
	else if(is_read){
		OneImage * omg = read1bitimage(input_file);
		Image *img2 = convertOneToImg(omg);
		imageOutput(img2, output_file);
		freeOneImage(omg);
		freeImage(img2);
	}
	else if(is_upgrade){ // filename filename_2 upgrade type
		if(file_version == NULL){
			file_version = ".0";
		}
		OneImage * omg = read1bitimage(input_file);
		oneBitWrite(omg, output_file,file_version);
		freeOneImage(omg);
	}
	else{
		fprintf(stderr,"Incorrect ordering of \n");
	}
	return 0;
}