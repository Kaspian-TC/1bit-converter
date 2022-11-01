#include <stdio.h>
#include "imgutils.h"
#include "1bit_funcs.h"
int main(int argc, char** argv){
	if(argc==1){
		fprintf(stderr,"Please include a path to a file\n");
		return 1;
	}
	if(strcmp(argv[3],"create") ==0){
		Image *img = readPPMimage(argv[1]);
		OneImage *omg = convertImgToOne(img);
		oneBitOutput(omg, argv[2]);
		freeOneImage(omg);
		freeImage(img);
	}
	else if(strcmp(argv[3],"read") ==0){
		OneImage * omg = read1bitimage(argv[1]);
		Image *img2 = convertOneToImg(omg);
		imageOutput(img2, argv[2]);
		freeOneImage(omg);
		freeImage(img2);
	}
	return 0;
}