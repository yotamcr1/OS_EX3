#include "factory.h"


int compare(const void * a, const void * b)
{
	return (*(int*)a - *(int*)b);
}


int* decompose_into_primary_numbers(int number, int* num_of_primary_numbers) {
	int primary_numbers_local[MAX_PRIMARY_NUMBERS];//check if there is maximum size? memory allocation?
	int i = 3, p = 0;
	while ((number % 2) == 0) {
		number = number / 2;
		primary_numbers_local[p] = 2;
		p++;
	}
	while (i <= sqrt(number)) {
		while ((number%i) == 0) {
			number = number / i;
			primary_numbers_local[p] = i;
			p++;
		}
		i = i + 2;
	}
	if (number > 2) {
		primary_numbers_local[p] = number;
		p++;
	}
	*num_of_primary_numbers = p;
	int* primary_numbers_global = (int*)malloc(p * sizeof(int));//have to free the pointer when exiting the program
	if (NULL == primary_numbers_global) {
		printf("failed to allocate memory to int array within decompose_into_primary_numbers. exit\n");
		return ERROR_CODE;
	}
	for (i = 0; i < p; i++) {
		primary_numbers_global[i] = primary_numbers_local[i];
	}
	return primary_numbers_global;
}


char* format_output_string(int* primary_numbers, int number, int num_of_primary_numbers) {
	int i = 0;
	char* output_string = (char*)malloc(MAX_OUTPUT_STR_LENGTH * sizeof(char));
	if (NULL == output_string) {
		printf("failed to allocate memory to char array within format_output_string. exit\n");
		return ERROR_CODE;
	}
	char str_number[MAX_NUM_BYTE];
	_itoa_s(number, str_number, MAX_NUM_BYTE, DECIMAL_BASE);
	strcpy_s(output_string, MAX_OUTPUT_STR_LENGTH * sizeof(char), "The prime factors of ");
	strcat_s(output_string, MAX_OUTPUT_STR_LENGTH * sizeof(char), str_number);
	strcat_s(output_string, MAX_OUTPUT_STR_LENGTH * sizeof(char), " are: ");
	for (i = 0; i < num_of_primary_numbers - 1; i++) {
		_itoa_s(primary_numbers[i], str_number, MAX_NUM_BYTE, DECIMAL_BASE);
		strcat_s(str_number, MAX_NUM_BYTE, ", ");
		strcat_s(output_string, MAX_OUTPUT_STR_LENGTH * sizeof(char), str_number);
	}
	_itoa_s(primary_numbers[i], str_number, MAX_NUM_BYTE, DECIMAL_BASE);
	strcat_s(output_string, MAX_OUTPUT_STR_LENGTH * sizeof(char), str_number);
	return output_string;
}

/*
char* initialize_array_of_priorities(FILE* fptr_priorities,int num_of_tasks, HANDLE hfile) {
	char* priorities = (char*)malloc(num_of_tasks * sizeof(char));
	char* priority_str = (char*)malloc(MAX_NUM_BYTE * sizeof(char));
	DWORD dwBytesRead;
	for (int i = 0; i < num_of_tasks; i++) {//
		if (ReadFile(hfile, priority_str, , &dwBytesRead, NULL)) {

		}
	}
}
*/


//the function create queue with the priorities from the priorities file
//
//
node* read_priorities_and_create_queue(FILE* fptr) {
	char task[MAX_TASK_LEN];
	char c;
	int i = 0, curr_offset;
	node* queue = NULL;
	for (c = getc(fptr); c != EOF; c = getc(fptr)) {
		task[i] = c;
		if (c == '\n') {
			task[i] = '\0';
			curr_offset = atoi(task);
			if (queue == NULL) {
				queue = InitializeQueue(curr_offset);
			}
			else
				queue = Push(queue, curr_offset);
			i = 0;
		}
		else
			i++;
	}
	return queue;
}
/*
static DWORD create_primary_thread(FILE* fptr_tasks, FILE* fptr_priorities, int num_of_tasks) {
	//HANDLE output_files_handles[MAX_THREADS];	//handles to output files, must set pointers correctly
	HANDLE hfile;
	hfile = CreateFileA(
		"Tasks.txt",
		GENERIC_WRITE, //Open file with write mode
		FILE_SHARE_WRITE, //the file should be shared by the threads.
		NULL, //default security mode
		OPEN_EXISTING, //the file is already exists
		FILE_ATTRIBUTE_NORMAL, //normal attribute
		NULL //not relevant for open file operations.
	);
	if (hfile == INVALID_HANDLE_VALUE)
		{
			printf("Can't open the Tasks inputfile. exit\n");
			exit(1);
		}
	node* queue = read_priorities_and_create_queue(fptr_priorities);
	//char* priorities = initialize_array_of_priorities(fptr_priorities,num_of_tasks, hfile);
	CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size
		read_priorities_and_create_queue,       // thread function name
		fptr_priorities,          // argument to thread function
		0,                      // use default creation flags
		NULL);   // NULL but have to check it
}
*/

void count_bytes_per_task(int* bytes_per_task, FILE* fptr) {
	int char_count = 0;
	int index = 0;
	char c;
	bytes_per_task[index] = char_count;
	index++;
	for (c = getc(fptr); c != EOF; c = getc(fptr)) {
		char_count++;
		if (c == '\n') {
			char_count++; // \n is 2 chars in read_file function
			bytes_per_task[index] = char_count;
			index++;
		}
	}
	fclose(fptr);
}


int main(int argc, char* argv[])
{
	FILE* fptr_tasks;
	FILE* fptr_priorities;
	int num_of_tasks = 0, num_of_threads = 0, i = 0, num_of_primary_numbers = 0, number_of_chars_within_tasks_file = 0;
	int is_open_tasks = fopen_s(&fptr_tasks, argv[1], "r");
	if (NULL == fptr_tasks || is_open_tasks != 0) {
		printf("Can't open the tasks input file \n");
		exit(1);
	}
	int is_open_priorities = fopen_s(&fptr_priorities, argv[2], "r");
	if (NULL == fptr_priorities || is_open_priorities != 0) {
		printf("Can't open the priorities input file \n");
		exit(1);
	}
	char temp_char = argv[3][0];
	while (temp_char != '\0') { //Translate number of tasks from string to integer
		num_of_tasks = num_of_tasks * 10 + (temp_char - '0');
		i++;
		temp_char = argv[3][i];
	}
	i = 0;
	temp_char = argv[4][0];
	while (temp_char != '\0') { //Translate number of threads from string to integer
		num_of_threads = num_of_threads * 10 + (temp_char - '0');
		i++;
		temp_char = argv[4][i];
	}
	//count_number_of_chars(fptr_tasks, &number_of_chars_within_tasks_file);
	int* bytes_per_task = (int*)malloc((num_of_tasks + 2) * sizeof(int));
	if (NULL == bytes_per_task) {
		printf("failed to allocate memory to int array within main. exit\n");
		return ERROR_CODE;
	}
	count_bytes_per_task(bytes_per_task, fptr_tasks);
	//printf("%d %d %d %d", bytes_per_task[0], bytes_per_task[1], bytes_per_task[2], bytes_per_task[3]);
	int* primary_numbers = decompose_into_primary_numbers(num_of_tasks, &num_of_primary_numbers);
	//printf("%d %d %d %d", primary_numbers[0], primary_numbers[1], primary_numbers[2], primary_numbers[3]);
	//int array[] = { 15,2, 8,7 };
	//qsort(array, 4, sizeof(int), compare);
	printf("%d %d %d %d\n", primary_numbers[0], primary_numbers[1], primary_numbers[2], primary_numbers[3]);
	qsort(primary_numbers, num_of_primary_numbers, sizeof(int), compare);
	char* output_string = format_output_string(primary_numbers, num_of_tasks, num_of_primary_numbers);
	printf("%s\n", output_string);
	//int x = create_primary_thread(fptr_tasks, fptr_priorities,num_of_tasks,number_of_chars_within_tasks_file);
	/*//function to check queue:
	node* new_node = InitializeQueue(8);
	printf("%d\n", new_node->offset_in_bytes);
	printf("%d\n", Top(new_node));
	new_node = Push(new_node, 9);
	PrintQueue(new_node);
	printf("%d\n", Empty(new_node));
	new_node = Pop(new_node);
	PrintQueue(new_node);
	new_node = Pop(new_node);
	PrintQueue(new_node);
	printf("%d\n", Empty(new_node));
	new_node = InitializeQueue(8);
	PrintQueue(new_node);
	new_node = DestroyQueue(new_node);
	PrintQueue(new_node);
	printf("%d\n", Empty(new_node));*/
	//node* queue = read_priorities_and_create_queue(fptr_priorities);
	int x = create_primary_thread(fptr_tasks, fptr_priorities, num_of_tasks);
	//PrintQueue(queue);
}


//static DWORD create_threads

