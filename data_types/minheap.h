#ifndef __MINHEAP_H__
#define __MINHEAP_H__

#include<stdlib.h>
#include<stdio.h>
#include<math.h>


// One node of the heap
typedef struct {
  int val;
  double priority;
} HeapElement;

// Array representation of the heap, with some metadata
typedef struct {
  int numItems;       // Number of items currently in the heap
  int maxSize;           // Total (maximum) size of the heap 
  
  HeapElement *arr;   // The actual array representing the minHeap
  int *indices;       // indices[key] = index of key in `arr`, or -1
} MinHeap;

// Allocate and free
MinHeap *newMinHeap(int maxSize);
void freeHeap(MinHeap *heap);

// Core heap functions...
void heapPush(MinHeap *heap, int val, double priority);
int heapExtractMin(MinHeap *heap, double *priority);
void heapDecreasePriority(MinHeap *heap, int val, double priority);
int heapSize(MinHeap *heap);
char isHeapEmpty(MinHeap *heap);
#endif // __MINHEAP_H__
