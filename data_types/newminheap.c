#include "minheap.h"
#include <stdlib.h>
#include <string.h>

#define PARENT_INDEX(X) ((X+1)/2)

// Allocate and free
MinHeap *newMinHeap(int maxSize){
	MinHeap *new_heap = malloc(sizeof(MinHeap));
	new_heap->numItems = 0;
	new_heap->maxSize = maxSize;
	
	new_heap->arr = calloc(sizeof(HeapElement),maxSize);
	new_heap->indices = calloc(sizeof(int),maxSize);
	memset(new_heap->indices, -1, sizeof(int));
	
	return new_heap;
}
void freeHeap(MinHeap *heap){
	free(heap->arr);
	free(heap->indices);
	free(heap);
}

static void swapElements(MinHeap * heap, int i, int j){
	heap->indices[heap->arr[i].val] = j;
	heap->indices[heap->arr[j].val] = i;

	HeapElement temp_element = heap->arr[i];
	heap->arr[i] = heap->arr[j];
	heap->arr[j] = temp_element;
}
// Core heap functions...
void heapPush(MinHeap *heap, int val, double priority){
	heap->arr[heap->numItems] = (HeapElement){val,priority};
	heap->indices[val] = heap->numItems;
	heap->numItems++;
	// heapify up
	int parent_index = PARENT_INDEX(heap->indices[val]);
	while (heap->arr[parent_index].priority > priority){ 
		// swap 
		int current_index = heap->indices[val];
		swapElements(heap, parent_index, current_index);
		parent_index = PARENT_INDEX(heap->indices[val]);
	}
}
int heapExtractMin(MinHeap *heap, double *priority);
void heapDecreasePriority(MinHeap *heap, int val, double priority);
int heapSize(MinHeap *heap){
	return heap->numItems;
}
char isHeapEmpty(MinHeap *heap){
	return heap->numItems == 0;
}

int main(){
	MinHeap * heap = newMinHeap(10);
	heapPush(heap, 10, 10.0);
}

