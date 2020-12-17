#include "factory.h"
//all the functions in this module are debuged

node* InitializeQueue(int offset) {//check if we dont need to seperate this from the alocate function
	node* new_queue = allocate_place_for_node(offset);
	return new_queue;
}

//the function allocate place for new node in the queue
//input: offset in byte of the priority in the tasks file
//output: pointer to the new node that allocated

node* allocate_place_for_node(int offset) {
	node *new_node = (node*)malloc(sizeof(node));
	if (new_node != NULL) {
		new_node->offset_in_bytes = offset;
		new_node->next = NULL;
	}
	return new_node;
}

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

//the function add an element to the end of the queue
//input: pointer to the queue struct
//output: pointer to the new queue with the new element in the end of it
node* Push(node* queue, int offset) {
	node *new_node = allocate_place_for_node(offset);
	if (queue == NULL) //if queue wasn't initialize
		return new_node;
	else {
		node* temp_node = queue;
		while (temp_node->next != NULL) {
			temp_node = temp_node->next;
		}
		temp_node->next = new_node;
		return queue;
	}
}

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

//this function is only for us to check our queue functions
void PrintQueue(node* queue)
{
	printf("[");
	while (queue != NULL) {
		printf("%d", queue->offset_in_bytes);
		if (queue->next != NULL)
			printf(", ");
		queue = queue->next;
	}
	printf("]\n");
}
