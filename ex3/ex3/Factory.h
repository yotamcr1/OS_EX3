#ifndef FACTORY_H
#define FACTORY_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>


#define ERROR_CODE ((int)(-1))
#define MAX_TASK_LEN 9
#define MAX_TIME_PER_THREAD 100000
#define MAX_PRIMARY_NUMBERS 30
#define MAX_PRIMARY_NUMBERS_ARRAY 90
#define MAX_OUTPUT_STR_LENGTH 126
#define MAX_THREADS 64
#define MAX_NUM_BYTE 10
#define DECIMAL_BASE 10
#define TIMEOUT 100000000 // have to check it again


//Global Variable Decleration of Factory.c:
char inout_file_address[MAX_OUTPUT_STR_LENGTH];

//struct declaration
/*
typedef struct thread {
	HANDLE output_file;
	int bytes_from_the_start;
	int task_number;
} thread_values, *Pthread_values;
*/


typedef struct node {
	int offset_in_bytes;
	struct node* next;
}node;

typedef struct queue_pointer {
	node* pq_head_node;
}queue_pointer;

typedef struct lock {
	int activeReaders;
	HANDLE Mutex; //mutex
	HANDLE turnstile;//mutex
	HANDLE roomEmpty; //semaphore
}lock;

typedef struct t {
	HANDLE inout_file;
	queue_pointer* p_q_head;
	lock* priority_lock;
	lock* lock_t;
	int max_length;

} thread_relevant_values, * Pthread_relevant_values;


//lock functions:
lock* InitializLock(int num_of_threads);
lock* allocate_place_for_lock(HANDLE mutex, HANDLE turnstile, HANDLE roomEmpty);
void read_lock(lock* lock);
void read_release(lock* lock);
void write_lock(lock* lock);
void write_release(lock* lock);
void DestroyLock(lock* lock);

//factory functions:
int* decompose_into_primary_numbers(int number, int* num_of_primary_numbers);
char* format_output_string(int* primary_numbers, int number, int num_of_primary_numbers);
int compare(const void * a, const void * b);
int count_bytes_per_task(/*int* bytes_per_task, */FILE* fptr);
queue_pointer* read_priorities_and_create_queue(FILE* fptr);

//queue functions:

node* allocate_place_for_node(int offset);
int Top(queue_pointer* pq);
node* InitializeQueue(int offset);
void Pop(queue_pointer* pq);
node* Push(queue_pointer* pq, int offset);
bool Empty(queue_pointer* pq);
node* DestroyQueue(queue_pointer* pq);



#endif