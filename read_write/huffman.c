#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
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
	// printf("start: head_id: %d head_priority: %f \n",head_id,head_priority);
	if(head_id < size){ //one of the leaf nodes
		assert(current_run != 1);
		encoded_bits[head_id] = current_run;
		// printf("head_id: %c current_run:",head_id);
		// printBinary(current_run);
		// printf("\n");
		return;
	}
	// Find the left and right then recurse, make sure to bit shift and add 1 
	// if right, 0 if left 1

	TreeNode head = combined_leaves[head_id-size];
	//  Call left child
	uint16_t left_run = current_run << 1; //bit shift left
	assignEncodedBits(encoded_bits,combined_leaves,size,head.left_id,
	head.left_priority,left_run);
	// Call right child
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
static int assignTreeNodes(TreeNode * tree,MinHeap * min_heap, const int heap_size,double * head_priority){
	int current_leaf_id = heap_size;
	// TODO: While loop should be in a function
	while(heapSize(min_heap) > 1){
		double leaf1_priority, leaf2_priority;
		int leaf1_id = heapExtractMin(min_heap,&leaf1_priority);
		int leaf2_id = heapExtractMin(min_heap,&leaf2_priority);
		printf("leaf1_id: %d leaf2_id: %d\n",leaf1_id,leaf2_id);
		printf("leaf1_priority: %.1f leaf2_priority: %.1f\n",leaf1_priority,leaf2_priority);
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
uint8_t* huffmanEncode(const uint8_t* data, const int byte_length,long* size){
	// Make heap_size = 256 because that is the max amount of possible values
	const int heap_size = 256;
	MinHeap * min_heap = createNewHuffmanHeap(heap_size,data,byte_length);
	/*MinHeap * min_heap = newMinHeap(heap_size);
	unsigned long number_of_hits[heap_size];
	// Sets number of hits to zero
	for (int i = 0; i < heap_size; i++) {
		number_of_hits[i] = 0;
	}
	// Adds all occurences of values
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
	}*/

	if(min_heap == NULL){
		fprintf(stderr,"All 256 bytes where detected."
		" Huffman encoding is redundant");
		return NULL;
	}
	TreeNode tree[heap_size];
	/*int current_leaf_id = heap_size;
	// TODO: While loop should be in a function
	while(heapSize(min_heap) > 1){
		double leaf1_priority, leaf2_priority;
		int leaf1_id = heapExtractMin(min_heap,&leaf1_priority);
		int leaf2_id = heapExtractMin(min_heap,&leaf2_priority);
		printf("leaf1_id: %d leaf2_id: %d\n",leaf1_id,leaf2_id);
		printf("leaf1_priority: %.1f leaf2_priority: %.1f\n",leaf1_priority,leaf2_priority);
		assert(leaf1_id != -1 && leaf2_id != -1);

		int new_leaf_id = current_leaf_id;
		TreeNode new_leaf = assignNewLeaf(new_leaf_id,leaf1_id,leaf2_id,
									leaf1_priority,leaf2_priority);
		combined_leaves[current_leaf_id-256] = new_leaf;
		current_leaf_id++;
		
		heapPush(min_heap,new_leaf.id,new_leaf.priority);
	}
	double head_priority;
	int head_id = heapExtractMin(min_heap,&head_priority);*/
	double head_priority;
	int head_id = assignTreeNodes(tree,min_heap,heap_size,&head_priority);
	Bitrun* encoded_bits[256];
	for (int i = 0; i < 256; i++)
	{
		encoded_bits[i] = createBitRun(NULL);
	}
	printTree(tree,256,head_id,0);
	assignEncodedBits(encoded_bits,tree,256,head_id,head_priority,1);
	freeHeap(min_heap);
	return NULL; // TODO: this should not be NULL
}
