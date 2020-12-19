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
	node* new_node = (node*)malloc(sizeof(node));
	if (new_node != NULL) {
		new_node->offset_in_bytes = offset;
		new_node->next = NULL;
	}
	return new_node;
}

//the function return the first element offset in the queue
//input: pointer to queue struct
//output: the offset of the first element in the queue

int Top(/*node* queue*/ queue_pointer* pq) {
	if(!Empty(/*queue*/ pq->pq_head_node))
		return /*queue->offset_in_bytes*/ pq->pq_head_node->offset_in_bytes;
	return ERROR_CODE;
}


//the function remove the first element from the queue
//input: pointe to the queue struct
//output: pointer to the second element in the queue (the queue without the first element)
node* Pop(/*node* queue*/ queue_pointer* pq) {
	if (NULL == /*queue*/ pq->pq_head_node)
		return NULL;
	node* temp_queue = pq->pq_head_node;
	pq->pq_head_node = pq->pq_head_node->next;//this line ensures that all thread will have the same pointer!
	return temp_queue; 
	/*
	if (NULL == queue)
	return NULL;
	node* temp_queue = queue;
	queue = queue->next;//this line ensures that all thread will have the same pointer!
	return temp_queue;
	*/
	//needs to free the return values!!!!
}

//the function add an element to the end of the queue
//input: pointer to the queue struct
//output: pointer to the new queue with the new element in the end of it
node* Push(queue_pointer* pq, int offset) {
	node* new_node = allocate_place_for_node(offset);
	if (Empty(pq->pq_head_node)) //if queue wasn't initialize
		return new_node;
	else {
		node* temp_node = pq->pq_head_node;
		while (temp_node->next != NULL) {
			temp_node = temp_node->next;
		}
		temp_node->next = new_node;
		return pq->pq_head_node;
	}
}

//the function return true if the queue is empty and false otherwise
//input: pointer to the queue struct
//output: TRUE if the queue is empty and FALSE if its not
bool Empty(queue_pointer* pq) {
	if (pq->pq_head_node == NULL)
		return true;
	return false;
}


//the function free all hte resources 
//input: pointer to the queue struct
//output: pointer to NULL 
node* DestroyQueue(queue_pointer* pq) {
	node* temp_node;
	while (pq->pq_head_node->next != NULL) {
		temp_node = pq->pq_head_node;
		pq->pq_head_node = pq->pq_head_node->next;
		free(temp_node);
	}
	pq->pq_head_node = NULL;
	return pq->pq_head_node;
}

//this function is only for us to check our queue functions
void PrintQueue(queue_pointer* pq)
{
	printf("[");
	while (pq->pq_head_node != NULL) {
		printf("%d", pq->pq_head_node->offset_in_bytes);
		if (pq->pq_head_node->next != NULL)
			printf(", ");
		pq->pq_head_node = pq->pq_head_node->next;
	}
	printf("]\n");
}