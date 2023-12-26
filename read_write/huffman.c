#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "../data_types/minheap.h"
#include "huffman.h"
#include "../data_types/bitrun.h"
typedef struct leaf
{
	int id;
	float priority;
	int left_id,right_id;
	float left_priority,right_priority;
}TreeNode;
#ifdef DEBUG
static void printBinary(uint16_t number){
	for(int i = 15;i>=0;i--){
		printf("%d",number>>i & 1);
	}
	return;
}
static void printLeaf(TreeNode leaf){
	printf("id: %d\n",leaf.id);
	printf("priority: %f\n",leaf.priority);
	printf("children: left: %d right: %d\n",leaf.left_id,leaf.right_id);
	printf("child_priorities: left: %f right:%f\n",leaf.left_priority,
	leaf.right_priority);

	return;
}
static void printTree(TreeNode* combined_leaves,int size,int head_id,int depth){
	if(head_id < size){ //one of the leaf nodes
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
#endif // DEBUG
static void printBinary(uint8_t number){
	for(int i = 7;i>=0;i--){
		printf("%d",number>>i & 1);
	}
	return;
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
static void assignEncodedBits(Bitrun * encoded_bits[],
			const TreeNode* combined_leaves, const int size, const int head_id,
			const float head_priority, const Bitrun * current_run,int depth){
	if(head_id < size){ //one of the leaf nodes
		assert(encoded_bits[head_id] == NULL);
		encoded_bits[head_id] = createAndCopyBitrun(current_run);
		return;
	}
	// Find the left and right then recurse, make sure to bit shift and add 1 
	// if right, 0 if left 1
	TreeNode head = combined_leaves[head_id-size];
	//  Call left child
	Bitrun * left_run = shiftAndAdd(createAndCopyBitrun(current_run),false); //bit shift left
	assignEncodedBits(encoded_bits,combined_leaves,size,head.left_id,
	head.left_priority,left_run,depth+1);
	freeBitrun(left_run);

	// Call right child
	Bitrun * right_run = shiftAndAdd(createAndCopyBitrun(current_run),true); //bit shift left and add 1
	assignEncodedBits(encoded_bits,combined_leaves,size,head.right_id,
	head.right_priority,right_run,depth+1);
	freeBitrun(right_run);
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
/**
 * @brief Creates a new heap and adds values that are not zero to the heap.
 * @param heap_size 
 * @param data 
 * @param data_length 
 * @return the new heap. If all values are detected or none are, return NULL
 */
static MinHeap * createNewHuffmanHeap(const int heap_size,const uint8_t* data,
 									  const int data_length){
	MinHeap * min_heap = newMinHeap(heap_size);
	unsigned long number_of_hits[heap_size];
	// Sets number of hits to zero
	for (int i = 0; i < heap_size; i++) {
		number_of_hits[i] = 0;
	}
	// Adds all occurences of values
	for(long i = 0; i < data_length; i++){
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
	if(total_zero_values == 0 || total_zero_values == heap_size){
		freeHeap(min_heap);
		return NULL;
	}
	return min_heap;
}
static int assignTreeNodes(TreeNode * tree,MinHeap * min_heap,
						   const int heap_size,double * head_priority){
	int current_leaf_id = heap_size;
	// TODO: While loop should be in a function
	while(heapSize(min_heap) > 1){
		double leaf1_priority, leaf2_priority;
		int leaf1_id = heapExtractMin(min_heap,&leaf1_priority);
		int leaf2_id = heapExtractMin(min_heap,&leaf2_priority);
		// printf("leaf1_id: %d leaf2_id: %d\n",leaf1_id,leaf2_id);
		// printf("leaf1_priority: %.1f leaf2_priority: %.1f\n",leaf1_priority,leaf2_priority);
		assert(leaf1_id != -1 && leaf2_id != -1);

		int new_leaf_id = current_leaf_id;
		TreeNode new_leaf = assignNewLeaf(new_leaf_id,leaf1_id,leaf2_id,
									leaf1_priority,leaf2_priority);
		tree[current_leaf_id-256] = new_leaf;
		current_leaf_id++;
		
		heapPush(min_heap,new_leaf.id,new_leaf.priority);
	}
	int head_id = heapExtractMin(min_heap,head_priority);
	return head_id;
}
uint8_t* compressData(Bitrun * encoded_bits[],const int encoded_bits_size,
					  const uint8_t* input_data,const int input_data_length,
					  long* size){
	uint8_t * output_data = malloc(sizeof(uint8_t)*input_data_length);
	long output_data_index = 0;
	for(int i = 0;i<input_data_length;i++){
		char current_char = input_data[i];
		const Bitrun * current_run = encoded_bits[(uint8_t)current_char];
		output_data_index = assignBitrunToMemory(current_run,output_data,
		output_data_index);
		
		if(output_data_index == -1){
			fprintf(stderr,"Error in compressData: output_data_index == -1\n");
			return NULL;
		}
	}
	*size = output_data_index;
	return output_data;
}
uint8_t* huffmanEncode(const uint8_t* data, const int data_length,long* size){
	// Make heap_size = 256 because that is the max amount of possible values
	const int heap_size = 256;
	// needs to be 2*heap_size because of the extra nodes that are created
	MinHeap * min_heap = createNewHuffmanHeap(heap_size*2,data,data_length);
	if(min_heap == NULL){
		fprintf(stderr,"All 256 bytes where detected."
		" Huffman encoding is redundant");
		return NULL;
	}
	TreeNode tree[heap_size];
	double head_priority;
	int head_id = assignTreeNodes(tree,min_heap,heap_size,&head_priority);
	freeHeap(min_heap); //shouldn't be required after

	// If uint256_t existed in c99, then this would be much better
	// Until I can figure that out, I will work with my current solution
	
	// Should be in another function
	Bitrun * encoded_bits[heap_size];
	for (int i = 0; i < heap_size; i++)
	{
		encoded_bits[i] = NULL;
	}
	Bitrun * starting_run = createBitRun();
	starting_run = shiftAndAdd(starting_run,true);
	assignEncodedBits(encoded_bits,tree,heap_size,head_id,head_priority,
	starting_run,0);
	freeBitrun(starting_run);
	
	#ifdef DEBUG
	for(int i = 0;i<heap_size;i++){
		if(encoded_bits[i] != NULL){
			printf("%c: ",i);
			printBitrun(encoded_bits[i]);
		}
	}
	#endif
	long compress_data_size;
	uint8_t* compressed_data = compressData(encoded_bits,
	heap_size,data,data_length,&compress_data_size);
	
	//free all bitruns 
	for(int i = 0;i<heap_size;i++){
		if(encoded_bits[i] != NULL){ // if null, then not assigned memory
			freeBitrun(encoded_bits[i]);
		}
	}
	size_t tree_size = sizeof(TreeNode) * heap_size;
	
	uint8_t* output_data = malloc(
		sizeof(uint8_t) + tree_size + sizeof(uint8_t)*data_length);

	output_data[0] = head_id;
	//longer names matter when mistakes cost the difference
	uint8_t tree_portion_of_byte_array = output_data + 1;
	memcpy(tree_portion_of_byte_array,tree,tree_size);
	uint8_t data_portion_of_byte_array = tree_portion_of_byte_array + tree_size;
	memcpy(data_portion_of_byte_array,compressed_data,compress_data_size);
	/* Takes byte array and assigns it to the tree_size, useful in reading code 
	TreeNode new_tree[heap_size];
	memcpy(new_tree,byte_array,tree_size);
	*/
	return output_data;
}
