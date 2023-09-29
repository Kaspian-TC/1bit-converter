/**
 *                        Min-Heaps
 * 
 * This is the Part 1 section of your coding assignment. You will  submit this
 * along with the Part 2 section when it is due, but it is recommended that you
 * complete this early on so you have time to work on second part of this
 * assignment (in `marcher.c` ).
 * 
 * Please make sure you read the blurb in `minheap.h` to ensure you understand 
 * how we are implementing min-heaps here and what assumptions / requirements 
 * are being made.
 * 
 * CSCB63 Winter 2022 - Coding Assignment 2
 * (c) Mustafa Quraish
 */

#include "minheap.h"
#include <assert.h>
/**
 * Allocate a new min heap of the given size.
 * 
 * TODO: 
 *  Allocate memory for the `MinHeap` object, and the 2 arrays inside it.
 *  `numItems` should initially be set to 0, and all the indices should be
 *   set to -1 to start with (since we don't have anything in the heap).
 */
MinHeap *newMinHeap(int size){
	MinHeap * heap = (MinHeap*) malloc(sizeof(MinHeap));
	heap->numItems=0;
	heap->maxSize = size;
	heap->arr = malloc(sizeof(HeapElement)*(size));
	heap->indices = malloc(sizeof(int)*(size));
	for(int i = 0;i<size;i++){
		heap->arr[i].val = -1; //both are from is from 0-MAXSIZE
		heap->indices[i] = -1; 
	}
	return heap; // Allocate and return heap.
}

/**
 * Swaps elements at indices `a` and `b` in the heap, and also updates their
 * indices. Assumes that `a` and `b` are valid.
 * 
 * NOTE: This is already implemented for you, no need to change anything.
 */
static void swap(MinHeap *heap, int a, int b) {
  // Swap the elements
  HeapElement temp = heap->arr[a];
  heap->arr[a] = heap->arr[b];
  heap->arr[b] = temp;
	
  // Refresh their indices
  heap->indices[heap->arr[a].val] = a;
  heap->indices[heap->arr[b].val] = b;
}

/**
 * Add a value with the given priority into the heap.
 * 
 * TODO: Complete this function, and make sure all the relevant data is updated
 *      correctly, including the `indices` array if you move elements around. 
 *      Make sure the heap property is not violated. 
 * 
 * You may assume the value does not already exist in the heap, and there is
 * enough space in the heap for it.
 */
void heapPush(MinHeap *heap, const int val, const double priority) {
	//insert new val into the last location in the heap (numItems)
	assert(val < heap->maxSize);
	heap->arr[heap->numItems].val = val;
	heap->arr[heap->numItems].priority = priority;
	// Refresh indices
	assert(heap->indices[val] == -1);
	heap->indices[val] = heap->numItems;
	int current_index = heap->numItems;
	while(current_index!=0)
	{
		if(heap->arr[(current_index-1)/2].priority > heap->arr[current_index].priority){ // if the head is greater than the current
			swap(heap,current_index,(current_index-1)/2); //swap until parent is smaller
			current_index = (current_index-1)/2 ; 
		}
		else
			break;
	}
	heap->numItems++;
  return; // Push value to heap before returning
}

static double doubleMin(double a, double b){//returns the min of two doubles
	if(a>b)
		return b;
	else
		return a;
}
/**
 * Extract and return the value from the heap with the minimum priority. Store
 *  the priority for this value in `*priority`. 
 * 
 * For example, if `10` was the value with the lowest priority of `1.0`, then
 *  you would have something that is equivalent to:
 *      
 *        *priority = 1.0;
 *        return 10;
 * 
 * TODO: Complete this function, and make sure all the relevant data is updated
 *      correctly, including the `indices` array if you move elements around. 
 *      Make sure the heap property is not violated. 
 * 
 * You may assume there is at least 1 value in the heap.
 */
int heapExtractMin(MinHeap *heap, double *priority) { 
	if(isHeapEmpty(heap)){
		return -1;
	}
  *priority = heap->arr[0].priority;  // Set correct priority  
	int val = heap->arr[0].val;
	int current_index = 0;
	heap->indices[val] = -1;
	heap->arr[0] = heap->arr[heap->numItems-1];
	heap->arr[heap->numItems-1].val = -1;
	heap->indices[heap->arr[0].val] = 0;
	heap->numItems--;
	
	int move_index; //index which gets moved
	//2*x+1 is left and 2*sub+2 is the right
	while((2*current_index+1<heap->numItems) && !(heap->arr[2*current_index+1].val ==-1 && heap->arr[2*current_index+2].val==-1) && (heap->arr[current_index].priority>doubleMin(heap->arr[2*current_index+1].priority, heap->arr[2*current_index+2].priority))){ //stops only when subnodes vals are -1 or when the priority is smaller than the subnodes or when there are no more subnodes to look to.
		if(heap->arr[2*current_index+2].val==-1 && heap->arr[2*current_index+1].priority<heap->arr[current_index].priority){// case where there is no right subnode and left subnode has a smaller priority
			move_index = 2*current_index+1;
		}
		else if(heap->arr[2*current_index+1].priority> heap->arr[2*current_index+2].priority) // case where left has higher priority
			move_index = 2*current_index+2; // swap right
		else if(heap->arr[2*current_index+1].priority<= heap->arr[2*current_index+2].priority){ // case where right has higher priority
			move_index = 2*current_index+1; // swap left
		}
		else{
			//case where one has value of -1 but a larger priority and the other has a smaller priority
			break;
		}
		swap(heap,current_index,move_index);
		current_index = move_index;
	}
  return val;         // Return correct value
}

/**
 * Decrease the priority of the given value (already in the heap) with the
 * new priority.
 * 
 * NOTE: You will find it helpful here to first get the index of the value
 *       in the heap from the `indices` array.
 * 
 * TODO: Complete this function, and make sure all the relevant data is updated
 *      correctly, including the `indices` array if you move elements around. 
 *      Make sure the heap property is not violated. 
 * 
 * You may assume the value is already in the heap, and the new priority is
 *  smaller than the old one (caller is responsible for ensuring this).
 */
void heapDecreasePriority(MinHeap *heap, int val, double priority) {
	//decreasing priority will mean that the sub priorities will always be greater so we don't need to worry about them
	//compare with parent and switch if smaller
	int current_index = heap->indices[val];
	heap->arr[current_index].priority = priority;
	while(current_index!=0 && heap->arr[(current_index-1)/2].priority > heap->arr[current_index].priority){ //continues when current is not head and parent is bigger
		swap(heap,current_index,(current_index-1)/2); //swap until parent is smaller
		current_index = (current_index-1)/2 ; 
	} 	
  return;   // Decrease priority before return
}

/**
 * Free the data for the heap. This won't be marked, but it is always good
 * practice to free up after yourself when using a language like C.
 */
void freeHeap(MinHeap *heap) {
	free(heap->arr);
	free(heap->indices);
	free(heap);
  return;   // Free heap and it's arrays
}
/** 
 * Returns the number of elements in the heap
*/
int heapSize(MinHeap *heap){
	return heap->numItems;
}
/** 
 * Returns the 
*/
char isHeapEmpty(MinHeap *heap){
	return heapSize(heap) == 0;
}
