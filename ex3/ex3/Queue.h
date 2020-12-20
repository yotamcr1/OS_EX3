#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdbool.h>
#include <math.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define ERROR_CODE ((int)(-1))

//struct definitions:

typedef struct node {
	int offset_in_bytes;
	struct node* next;
}node;

typedef struct queue_pointer {
	node* pq_head_node;
}queue_pointer;


//queue functions declaration:

//the function allocate place the queue
//output: pointer to the new queue that allocated
queue_pointer* InitializeQueue();

//the function allocate place for new node in the queue
//input: offset in byte of the priority in the tasks file
//output: pointer to the new node that allocated
node* allocate_place_for_node(int offset);

//the function return the first element offset in the queue
//input: pointer to queue struct
//output: the offset of the first element in the queue
int Top(queue_pointer* pq);

//the function remove the first element from the queue
//input: pointe to the queue struct
void Pop(queue_pointer* pq);

//the function add an element to the end of the queue
//input: pointer to the queue struct
//output: pointer to the new queue with the new element in the end of it
node* Push(queue_pointer* pq, int offset);

//the function return true if the queue is empty and false otherwise
//input: pointer to the queue struct
//output: TRUE if the queue is empty and FALSE if its not
bool Empty(queue_pointer* pq);

//the function free all the resources 
//input: pointer to the queue struct
//output: pointer to NULL 
void DestroyQueue(queue_pointer* pq);

#endif
