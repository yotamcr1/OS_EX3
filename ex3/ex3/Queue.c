#include "Queue.h"

queue_pointer* InitializeQueue() {
	queue_pointer* pq = (queue_pointer*)malloc(sizeof(queue_pointer));
	return pq;
}

node* allocate_place_for_node(int offset) {
	node* new_node = (node*)malloc(sizeof(node));
	if (new_node != NULL) {
		new_node->offset_in_bytes = offset;
		new_node->next = NULL;
	}
	return new_node;
}

int Top(queue_pointer* pq) {
	if (NULL == pq) {
		printf("The pq headr is Null within queue Top function. exit\n");
		return ERROR_CODE;
	}
	if (!Empty(pq))
		return pq->pq_head_node->offset_in_bytes;
	return ERROR_CODE;
}

void Pop(queue_pointer* pq) {
	if (NULL == /*queue*/ pq->pq_head_node)
		return NULL;
	node* temp_queue = pq->pq_head_node;
	pq->pq_head_node = pq->pq_head_node->next;//this line ensures that all thread will have the same pointer!
	free(temp_queue);
}

node* Push(queue_pointer* pq, int offset) {
	node* new_node = allocate_place_for_node(offset);
	if (Empty(pq)) //if queue wasn't initialize
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

bool Empty(queue_pointer* pq) {
	if (NULL == pq) {
		printf("pq is Null within Empty function. \n");
		//TBD: return indication? 
	}
	if (pq->pq_head_node == NULL)
		return true;
	return false;
}

void DestroyQueue(queue_pointer* pq) {
	node* temp_node;
	if (NULL == pq) {
		printf("pq is Null within destrot queue\n");
		return;
	}
	while (!Empty(pq)) {
		Pop(pq);
	}
	free(pq);
	pq = NULL;
}

void PrintQueue(queue_pointer* pq)
{
	node* temp_node = pq->pq_head_node;
	printf("[");
	while (temp_node != NULL && temp_node->offset_in_bytes != NULL) {
		printf("%d", temp_node->offset_in_bytes);
		if (temp_node->next != NULL)
			printf(", ");
		temp_node = temp_node->next;
	}
	printf("]\n");
}