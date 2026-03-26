#include <stdbool.h>
#include <stdio.h>
// #include "imgutils.h"
#include "1bit_funcs.h" 
#include "dither_funcs.h"
#include <string.h> 
#include "read_write/1bit_read_functions.h"
#include "read_write/1bit_write_functions.h"
#include <argp.h>

char * toUpper(char * s){ //changes the string to be uppercase only
	for(int i = 0; s[i]!='\0';i++){
		if(s[i]>='a' && s[i]<='z'){
			s[i] -= 32; //converts to upper case letter
		} 
	}
	return s;
}
bool fileIsReadable(char *filename){// return 1 if valid, 0 otherwise
	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		return false;
	}
	fclose(f);
	return true;	
}
static int createFile(bool is_dithered,char * input_file, char * dither_type,  char * file_version,char * output_file){
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
		}
		else if(strcmp(upper_dither_type,"ATKINSON")==0){ 
			ditherAtkinson(img,1);
		}
		else if(strcmp(upper_dither_type,"JARVIS-JUDICE-NINKE")==0 || strcmp(upper_dither_type,"JJN")==0){ 
			ditherJarvisJudiceNinke(img,1);
		}
		else if(strcmp(upper_dither_type,"BAYER-0")==0){
			imgBayerZero(img);
		}
		else if(strcmp(upper_dither_type,"BAYER-1")==0){
			imgBayerOne(img);
		}
		else if(strcmp(upper_dither_type,"BAYER-2")==0){
			imgBayerTwo(img);
		}
		else if(strcmp(upper_dither_type, "RANDOM")==0){
			randomDither(img);
		}
		else{ // error, end program
			fprintf(stderr,"invalid dithering type specified\n");
			freeImage(img);
			return 1;
		}
	}
	else{
		averageColourImage(img);
	}
	omg = convertImgToOne(img); //threshhold dithers without avg
	
	if(file_version != NULL){
		oneBitWrite(omg, output_file,file_version);
	}
	else{
		oneBitOutput(omg, output_file);
	}
	freeOneImage(omg);
	freeImage(img);
	return 0;
	 
}

struct arguments
{
	char * input_file,* output_file, *dither_type,* file_version;
	char field;
	bool is_dithered;
};
static int parse_opt(int key, char * arg, struct argp_state *state){
	struct arguments * arguments = state->input;
	switch (key) {
	case 'c':
		{
			if (arguments->field != 0){
				argp_failure(state, 1, 0, "too many flags have been passed");
				break;
			}
			else{
				arguments->field = 'c';
			}
			if (arg == NULL){
				arguments->file_version = ".0";
			}
			else{
				arguments->file_version = arg;
				printf("in c:%s",arg);
			}
		}
		break;
	case 'u':
		{
			if (arguments->field != 0){
				argp_failure(state, 1, 0, "too many flags have been passed");
				break;
			}
			arguments->field = 'u';
			arguments->file_version = arg;
		}
		break;
	case 'd':
		{
			arguments->is_dithered = true;
			arguments->dither_type = arg;
		}
	case 'r':
		{
			if (arguments->field != 0){
				argp_failure(state, 1, 0, "too many flags have been passed");
				break;
			}
			if (arg !=NULL){
				argp_failure(state, 1, 0, "no flag arguments should be passed to read");
				break;
			}
			arguments->field = 'r';
		}
		break;
	case ARGP_KEY_ARG:
		{
			if (arguments->input_file == NULL){
				arguments->input_file = arg;
			}
			else if (arguments->output_file == NULL){
				arguments->output_file = arg;
			}
		}
		break;
    case ARGP_KEY_END:
        {
			if (!arguments->input_file){
				argp_failure(state, 1, 0, "did not include input file");
			}
			if (!arguments->output_file){
				argp_failure(state, 1, 0, "did not include output file");
			}
			if (arguments->field == 0){
				argp_failure(state, 1, 0, "too few flags have been passed");
			}
        }
        break;
	}
	return 0;
}
int main(int argc, char** argv){
	/* -r : read flag
	-c : create flag
	-u : upgrade flag
	-i : precides the two file paths
	-d : precides dithering type
	-co : colour flag for create, followed by two hexidecimals in form #XXXXXX
	(unimplemented)
	*/

	bool is_read = 0;
	bool is_create = 0;
	bool is_upgrade = 0;
	bool is_dithered = 0;
	
	char * dither_type;
	char * input_file;
	char * output_file;
	char * file_version = NULL;
	struct argp_option options[] =
	{
		{ "read", 'r',0,OPTION_ARG_OPTIONAL,"Create an image file from a 1bit file"},
		{ "create", 'c',"VERSION",OPTION_ARG_OPTIONAL,"Create a 1bit file from another image file type"},
		{ "upgrade", 'u',"VERSION",0,"Create a file with a different level of compression"},
		{ "dither", 'd',"DITHERTYPE",0,"Select a dithering algorithm"},
		{0}
	};
	struct arguments arguments = {0};
	struct argp argp = {options,parse_opt,"INPUTFILE OUTPUTFILE"};
	int argp_error = argp_parse(&argp, argc, argv, 0, 0, &arguments);
	if (argp_error != 0){
		return argp_error;
	}
	is_read = arguments.field == 'r';
	is_create = arguments.field == 'c';
	is_upgrade = arguments.field == 'u';
	is_dithered = arguments.is_dithered;
	dither_type = arguments.dither_type;
	input_file = arguments.input_file;
	output_file = arguments.output_file;
	file_version = arguments.file_version;
	
	if(is_read + is_create + is_upgrade != 1){
		fprintf(stderr,"must include -r,-c, or -u (but only one)\n");
		return 1;
	}
	if(!(fileIsReadable(input_file))){
		fprintf(stderr,"Unable to open file %s. Check the path.\n",input_file);
		return 1;
	}
	
	if(is_create){
		int is_error = createFile(is_dithered,input_file,dither_type,file_version,output_file);
		if (is_error == 1){
			return 1;
		}
	}
	else if(is_read){
		OneImage * omg = read1bitimage(input_file);
		Image *img2 = convertOneToImg(omg);
		writeImage(img2, output_file);
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
