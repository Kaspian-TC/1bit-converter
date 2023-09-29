#include "1bit_write_functions.h"
// #include "../data_types/minheap.h"
#include "huffman.h"
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" //this should only be included here since this is the WRITE file
#endif
/** 
 * 0 is the left leaf while 1 is the right leaf
*/
/*
typedef struct leaf
{
	int id;
	float priority;
	int left_id,right_id;
	float left_priority,right_priority;
}TreeNode;
void printBinary(uint16_t number){
	for(int i = 15;i>=0;i--){
		printf("%d",number>>i & 1);
	}
	return;
}

void printLeaf(TreeNode leaf){
	printf("id: %d\n",leaf.id);
	printf("priority: %f\n",leaf.priority);
	printf("children: left: %d right: %d\n",leaf.left_id,leaf.right_id);
	printf("child_priorities: left: %f right:%f\n",leaf.left_priority,
	leaf.right_priority);

	return;
}
void printTree(TreeNode* combined_leaves,int size,int head_id,int depth){
	if(head_id < size){ //one of the leaf nodes
		// printf("\n");
		printf("leaf: %c\n",head_id);
		return;
	}
	TreeNode head = combined_leaves[head_id-size];
	printf("%03d ",head_id);
	printTree(combined_leaves,size,head.left_id,depth+1);
	
	for(int i = 0;i<depth;i++){
		printf("    ");
	}
	printTree(combined_leaves,size,head.right_id,depth+1);
	return;
}*/
static void copyData(uint8_t* dest, const uint8_t* src,long length){
	for(int i = 0;i<length;i++){
		dest[i] = src[i];
	}
	return;
}
static void printBinary(uint16_t number){
	for(int i = 15;i>=0;i--){
		printf("%d",number>>i & 1);
	}
	return;
}
static uint8_t* runLengthEncode(const uint8_t* data,int x_size,int y_size,long* size){ //encodes the data with run length encoding
	//if there is negative compression, makes new_data identical to data
	/*aaaabbbbcdddddee will become aa4bb4cdd5ee2 unless there is negative compression in which case it will exit with NULL*/
	int length = ceil((float)(x_size* y_size)/8);
	uint8_t* new_data = calloc((size_t)length, sizeof(uint8_t));
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

void oneBitWrite(OneImage *omg, char *filename, char *type) { //outputs to 1bit file format
  if (omg == NULL || !(strcmp(type,"")==0 || strcmp(type,".0")==0 ||
   strcmp(type,".1")==0 || strcmp(type,".2")==0)){
	  fprintf(stderr, "oneBitWrite(): wrong type\n");
  }
    if (omg->data != NULL) {
      FILE *f = fopen(filename, "wb+");
	  
      if (f == NULL) {
        fprintf(stderr, "Unable to open file %s in oneBitWrite.\n", filename);
        return;
      }
      fprintf(f, "1bit%s\n",type);
      fprintf(f, "%d %d\n", omg->sx, omg->sy);
	  if(strcmp(type,"")==0|| strcmp(type,".0")==0){
		fwrite(omg->data, (size_t)ceil((float)(omg->sx* omg->sy)/8),
		 sizeof(uint8_t), f);
	  }
	  else if(strcmp(type,".1")==0){
		uint8_t *data;
		long size;
		size = 0;
		data = runLengthEncode(omg->data,omg->sx,omg->sy,&size);
		fwrite(data, (size_t)size, sizeof(uint8_t), f);
		if(size==ceil((float)(omg->sx* omg->sy)/8)){
			fprintf(stderr, "encoding resulted in negative compression, will"
			" now regularly write\n");
			rewind(f);
			fprintf(f, "1bit.0\n");
		}
		free(data);
	  }
	  else if (strcmp(type,".2")==0) // huffman encoding
	  {
		uint8_t *data;
		long size;
		size = 0;
		const char *inputString = "A_DEAD_DAD_CEDED_A_BAD_BABE_A_BEADED_ABACA_BED";
		uint8_t byteArray [strlen(inputString)];
		for(int i = 0;i<strlen(inputString);i++){
			byteArray[i] = (uint8_t) inputString[i];
		}
		data = huffmanEncode(byteArray,strlen(inputString),&size);
		for(int i = 0;i<size/8;i++){
			printBinary(data[i]);
			// printf("%d ",data[i]);
		}
		free(data);
	  }
	  printf("before fclose\n");
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
void writeImage(Image *img, char *filename) {
  if (img != NULL)
    if (img->data != NULL) {
		//stride_in_bytes is the width * 3 (3 for the number of bytes per pixel)
		int return_value = stbi_write_png(filename,img->sx, img->sy, 3,
		 img->data, img->sx * 3); 
		if(return_value == 0){
			fprintf(stderr, "something wrong with writing png\n");
		}
		return;
    }
  fprintf(stderr, "imageOutput(): Specified image is empty. Nothing output\n");
  return;
}
