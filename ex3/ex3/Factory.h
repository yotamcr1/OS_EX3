#ifndef FACTORY_H
#define FACTORY_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include<search.h>


#define ERROR_CODE ((int)(-1))
#define MAX_TASK_LEN 9
#define MAX_FILE_LENGTH 200
#define MAX_TIME_PER_THREAD 10000
#define MAX_PRIMARY_NUMBERS 30
#define MAX_PRIMARY_NUMBERS_ARRAY 90
#define MAX_OUTPUT_STR_LENGTH 126
#define MAX_THREADS 64
#define MAX_NUM_BYTE 10
#define DECIMAL_BASE 10

//struct declaration
/*
typedef struct thread {
	HANDLE output_file;
	int bytes_from_the_start;
	int task_number;
} thread_values, *Pthread_values;
*/

typedef struct t {
	char* inout_address;
	HANDLE inout_file;
	DWORD orig_file_size;
	int offset;
	int max_length;
} thread_relevant_values, * Pthread_relevant_values;


typedef struct node {
	int offset_in_bytes;
	struct node* next;
}node;


int* decompose_into_primary_numbers(int number, int* num_of_primary_numbers);
char* format_output_string(int* primary_numbers, int number, int num_of_primary_numbers);
int compare(const void * a, const void * b);
int count_bytes_per_task(int* bytes_per_task, FILE* fptr);
node* read_priorities_and_create_queue(FILE* fptr);

//queue functions:

node* allocate_place_for_node(int offset);
int Top(node* queue);
node* InitializeQueue(int offset);
node* Pop(node* queue);
node* Push(node* queue, int offset);
bool Empty(node* queue);
node* DestroyQueue(node* queue);
#endif