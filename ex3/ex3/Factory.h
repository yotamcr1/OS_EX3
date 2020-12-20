#ifndef FACTORY_H
#define FACTORY_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include "Queue.h"
#include "Lock.h"

#define ERROR_CODE ((int)(-1))
#define MAX_TASK_LEN 9
#define MAX_TIME_PER_THREAD 100000
#define MAX_PRIMARY_NUMBERS 30
#define MAX_PRIMARY_NUMBERS_ARRAY 90
#define MAX_OUTPUT_STR_LENGTH 126
#define MAX_THREADS 64
#define MAX_NUM_BYTE 10
#define DECIMAL_BASE 10

//struct definition:

typedef struct t {
	HANDLE inout_file;
	queue_pointer* p_q_head;
	lock* priority_lock;
	lock* lock_t;
	int max_length;
} thread_relevant_values, *Pthread_relevant_values;


//factory functions:

//the function decompose a number to primary numbers
//input: array of ints, the number to be decompose into primary numbers
//output: array of ints contains the primary numbers 
int* decompose_into_primary_numbers(int number, int* num_of_primary_numbers);

//the function creates an output srting with the desired format from the instructions
//input: array of primary numbers, the task number, the num of primary numbers
//output: string with the desired format to be print to the tasks file
char* format_output_string(int* primary_numbers, int number, int num_of_primary_numbers);

//the function open handle to the task file for reading and writing to it
//input: pointer to tasks file
//output: handle for read and write to tasks file
HANDLE open_inout_handle_read_write(char* inout_address);

//the functuion get the size of the input file
//input: handle to file
DWORD get_file_orig_size(HANDLE inout_file);

//the function execute tasks from the input task file until the priorities queue is empty
//input:all the p relevan values per one thread
//output: return 0 if the thread work succeeded
static DWORD WINAPI write_output_file_per_thread(LPVOID lpParam);

//the function creates the threads and intialize their parameters
//input: max length of line within the task file, pionter to the queue struct, number of thread from command line
static DWORD create_threads(int max_length, queue_pointer* q_head, int num_of_threads);

//the function compares between 2 ints
//input: 2 pointerrs to ints
//output: rteurn 1 if a is bigger then b
int compare(const void * a, const void * b);

//the function calculates the max line length in the file
//input: pointer to the tasks file
//output: the length of the longest line in the file
int calculate_max_line(FILE* fptr);

//the function create queue with the priorities from the priorities file
//input: number of tasks within priorities file and file pointer to priorities file
//output: queue struct with all the offset bytes from the priorities file
queue_pointer* read_priorities_and_create_queue(FILE* fptr, int num_of_task);

//the function free all the resources and exit the program
//input: pointers to the 2 lock structs and one queue struct
void free_resorces_and_kill_the_program(lock* p_lock, lock* prior_lock, queue_pointer* pq);

//the function initialize the global path for the file
//input: pointer to file address, and file path
void Initialize_global_file_path(char* inout_file_address, char* file_path);

//Command line inputs: missions file, priority file, number of missions(equal to number of lines in the input files), number of threads.
int main(int argc, char* argv[]);
#endif