#include "1bit_write_functions.h"
#include "../data_types/minheap.h"
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" //this should only be included here since this is the WRITE file
#endif
/** 
 * 0 is the left leaf while 1 is the right leaf
*/
typedef struct leaf
{
	int id;
	float priority;
	// int children[2];
	int left_id,right_id;
	float left_priority,right_priority;
	// float child_priorities[2];
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
}
static void copyData(uint8_t* dest, const uint8_t* src,long length){
	for(int i = 0;i<length;i++){
		dest[i] = src[i];
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

/**
 * @brief assignEncodedBits holds a run (current_run) that represents the 
 * current run STARTING AFTER the first 1. ie 00101001 represents 01001. 8 bit 
 * runs will have to use 9 bits.
 * @param encoded_bits 
 * @param number_of_hits 
 * @param combined_leaves 
 * @param size 
 * @param head_id 
 * @param head_priority 
 * @param current_run 
 */
static void assignEncodedBits(uint16_t * encoded_bits,
			TreeNode* combined_leaves, int size,int head_id,float head_priority,
			uint16_t current_run){
	printf("start: head_id: %d head_priority: %f \n",head_id,head_priority);
	if(head_id < size){ //one of the leaf nodes
		assert(current_run != 1);
		encoded_bits[head_id] = current_run;
		printf("head_id: %c current_run:",head_id);
		printBinary(current_run);
		printf("\n");
		return;
	}
	/* Find the left and right then recurse, make sure to bit shift and add 1 if right, 0 if left */

	TreeNode head = combined_leaves[head_id-size];
	/* Call left child */
	uint16_t left_run = current_run << 1 | 0; //bit shift left and add 0
	// printf("head_id: %d left_id: %d, right_id:%d ",head.id,head.left_id,head.right_id);
	// printBinary(current_run);
	// printf("\n");
	assignEncodedBits(encoded_bits,combined_leaves,size,head.left_id,
	head.left_priority,left_run);
	/* Call right child */
	uint16_t right_run = current_run << 1 | 1; //bit shift left and add 1
	assignEncodedBits(encoded_bits,combined_leaves,size,head.right_id,
	head.right_priority,right_run);
	
	return;
}
/**
 * @brief This function creates a new leaf and returns the leaf value. It makes
 * sure that the leaf with the higher priority is on the right.
 * @param new_leaf_id 
 * @param leaf1_id 
 * @param leaf2_id 
 * @param leaf1_priority 
 * @param leaf2_priority 
 * @return the new leaf
 */
static TreeNode assignNewLeaf(const int new_leaf_id,const int leaf1_id,
						const int leaf2_id,
						const float leaf1_priority,
						const float leaf2_priority){
	float new_leaf_priority = leaf1_priority + leaf2_priority;
	TreeNode new_leaf;
	
	if(leaf1_priority > leaf2_priority){
		new_leaf = (TreeNode) {
			new_leaf_id, // int id
			new_leaf_priority, // float priority;
			leaf2_id,leaf1_id, // int left_id,right_id;
			leaf2_priority,leaf1_priority // float left_priority,right_priority;
		};
	}
	else{
		new_leaf = (TreeNode) {
			new_leaf_id, 
			new_leaf_priority,
			leaf1_id,leaf2_id,
			leaf1_priority,leaf2_priority
		};
	}
	return new_leaf;
}
static uint8_t* huffmanEncode(const uint8_t* data,int byte_length,long* size){
	const int length = byte_length;
	/* Make heap_size =2*256 because that is the max amount of possible  */
	const int heap_size = 256;
	MinHeap * min_heap = newMinHeap(heap_size);
	unsigned long number_of_hits[heap_size];
	/* Sets number of hits to zero */
	for (int i = 0; i < heap_size; i++) {
		number_of_hits[i] = 0;
	}
	/* Adds all occurences of values */
	for(long i = 0; i < length; i++){
		number_of_hits[data[i]]++;
	}
	unsigned long total_zero_values = 0;
	for (int i = 0; i < heap_size; i++) {
		if(number_of_hits[i] > 0){
			// printf("%d: %ld\n",i,number_of_hits[i]);
			heapPush(min_heap,i,number_of_hits[i]);
		}
		else{
			total_zero_values++;
		}
	}
	if(total_zero_values == 0){
		fprintf(stderr,"All 256 bytes where detected."
		" Huffman encoding is redundant");
		return NULL;
	}
	TreeNode combined_leaves[256];
	int current_leaf_id = 256;
	// TODO: While loop should be in a function
	while(heapSize(min_heap) > 1){
		double leaf1_priority, leaf2_priority;
		int leaf1_id = heapExtractMin(min_heap,&leaf1_priority);
		int leaf2_id = heapExtractMin(min_heap,&leaf2_priority);
		printf("leaf1_id: %d leaf2_id: %d\n",leaf1_id,leaf2_id);
		printf("leaf1_priority: %.1f leaf2_priority: %.1f\n",leaf1_priority,leaf2_priority);
		assert(leaf1_id != -1 && leaf2_id != -1);

		int new_leaf_id = current_leaf_id;
		/*float new_leaf_priority = right_leaf_priority + left_leaf_priority;
		TreeNode new_leaf;
		if(right_leaf_priority > left_leaf_priority){
			new_leaf = (TreeNode) {
			new_leaf_id, 
			new_leaf_priority,
			{left_leaf_id,right_leaf_id},
			{left_leaf_priority,right_leaf_priority}
			};
		}
		else{
			new_leaf = (TreeNode) {
			new_leaf_id, 
			new_leaf_priority,
			{right_leaf_id,left_leaf_id},
			{right_leaf_priority,left_leaf_priority}
			};
		}*/
		TreeNode new_leaf = assignNewLeaf(new_leaf_id,leaf1_id,leaf2_id,
									leaf1_priority,leaf2_priority);
		combined_leaves[current_leaf_id-256] = new_leaf;
		current_leaf_id++;
		
		// printLeaf(new_leaf);
		heapPush(min_heap,new_leaf.id,new_leaf.priority);
		
	}
	double head_priority;
	int head_id = heapExtractMin(min_heap,&head_priority);
	uint16_t encoded_bits[256];
	for (int i = 0; i < 256; i++)
	{
		encoded_bits[i] = 0;
	}
	printTree(combined_leaves,256,head_id,0);
	assignEncodedBits(encoded_bits,combined_leaves,256,head_id,head_priority,1);
	/*for(int index = 0; index < 256; index++){
		if(number_of_hits[index] != 0){
			printf("%d, %d:",index,number_of_hits[index]);
			printBinary(encoded_bits[index]);
			printf("\n");
			// printf("%d: %d\n",index,encoded_bits[index]);
		} 
	}*/
	return NULL; // TODO: this should not be NULL
}
void oneBitWrite(OneImage *omg, char *filename, char *type) { //outputs to 1bit file format
  if (omg == NULL || !(strcmp(type,"")==0 || strcmp(type,".0")==0 || strcmp(type,".1")==0 || strcmp(type,".2")==0)){
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
	  else if (strcmp(type,".2")==0)
	  {
		uint8_t *data;
		long size;
		size = 0;
		const char *inputString = "A_DEAD_DAD_CEDED_A_BAD_BABE_A_BEADED_ABACA_BED";
		uint8_t byteArray [strlen(inputString)];
		for(int i = 0;i<strlen(inputString);i++){
			byteArray[i] = (uint8_t) inputString[i];
		}
		// data = huffmanEncode(omg->data,getOneImageByteLength(omg),&size);
		data = huffmanEncode(byteArray,strlen(inputString),&size);
		for(int i = 0;i<size;i++){
			printf("%d\n",data[i]);
		}
		printf("exited properly\n");
		exit(0);
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
void writeImage(Image *img, char *filename) {
  if (img != NULL)
    if (img->data != NULL) {
		int return_value = stbi_write_png(filename,img->sx, img->sy, 3, img->data, img->sx * 3); //stride_in_bytes is the width * 3 (3 for the number of bytes per pixel)
		if(return_value == 0){
			fprintf(stderr, "something wrong with writing png\n");
		}
		return;
    }
  fprintf(stderr, "imageOutput(): Specified image is empty. Nothing output\n");
  return;
}
