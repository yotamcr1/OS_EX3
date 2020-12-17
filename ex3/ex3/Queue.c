#include "factory.h"
//all the functions in this module are debuged

node* InitializeQueue(int offset) {//check if we dont need to seperate this from the alocate function
	node* new_queue = allocate_place_for_node(offset);
	return new_queue;
}

//the function allocate place for new node in the queue
//input: offset in byte of the priority in the tasks file
//output: pointer to the new node that allocated

#define ERROR_CODE ((int)(-1))
#define MAX_TASK_LEN 9
#define MAX_PRIMARY_NUMBERS 30
#define MAX_PRIMARY_NUMBERS_ARRAY 90
#define MAX_OUTPUT_STR_LENGTH 126
#define MAX_THREADS 64
#define MAX_NUM_BYTE 10
#define DECIMAL_BASE 10
#define TIMEOUT 10 // have to check it again

//the function return the first element offset in the queue
//input: pointer to queue struct
//output: the offset of the first element in the queue

int Top(node* queue) {
	return queue->offset_in_bytes;
}


//the function remove the first element from the queue
//input: pointe to the queue struct
//output: pointer to the second element in the queue (the queue without the first element)
node* Pop(node* queue) {
	node* temp_node;
	temp_node = queue->next;
	free(queue);
	return temp_node;
}

//queue functions:

//the function return true if the queue is empty and false otherwise
//input: pointer to the queue struct
//output: TRUE if the queue is empty and FALSE if its not
bool Empty(node* queue) {
	if (queue == NULL)
		return true;
	return false;
}


//the function free all hte resources 
//input: pointer to the queue struct
//output: pointer to NULL 
node* DestroyQueue(node* queue) {
	node* temp_node;
	while (queue->next != NULL) {
		temp_node = queue;
		queue = queue->next;
		free(temp_node);
	}
	queue = NULL;
	return queue;
}

#endif