#include "factory.h"


int compare(const void * a, const void * b)
{
	return (*(int*)a - *(int*)b);
}


int* decompose_into_primary_numbers(int number, int* num_of_primary_numbers) {
	int* primary_numbers_local = (int*)malloc(2*(ceil(sqrt(number)) + 1) * sizeof(int)); //Upper bond for number of divisidor of n is n^0.5 * 2
	//math proof for that can be find here https://math.stackexchange.com/questions/1053062/upper-limit-for-the-divisor-function
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
	free(primary_numbers_local);
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
	strcat_s(output_string, MAX_OUTPUT_STR_LENGTH * sizeof(char), "\n");

	return output_string;
}

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

//we should  check if its necessary to calculate the absoulut values of the offsets. 
int count_bytes_per_task(int* bytes_per_task, FILE* fptr) {
	int char_count = 0;
	int max_length = 1; 
	int index = 0;
	char c;
	bytes_per_task[index] = char_count;
	index++;
	for (c = getc(fptr); c != EOF; c = getc(fptr)) {
		char_count++;
		if (c == '\n') {
			char_count++; // \n is 2 chars in read_file function
			if (max_length < char_count)
				max_length = char_count;
			bytes_per_task[index] = char_count;
			index++;
		}
	}
	fclose(fptr);
	return max_length;
}

HANDLE open_inout_handle_read_per(char* inout_address) {
	HANDLE inout_file;
	inout_file = CreateFileA(
		inout_address,
		GENERIC_READ, //Open file with write read
		FILE_SHARE_READ, //the file should be shared by the threads.
		NULL, //default security mode
		OPEN_ALWAYS, //The files are already created, so files are exist.
		FILE_ATTRIBUTE_NORMAL, //normal attribute
		NULL);//not relevant for open file operations. 
	if (inout_file == INVALID_HANDLE_VALUE) {
		printf("Can't open the file for read within open_inout_handle_read_per func .\n");
		/// neeeeed to do things here
	}
	return inout_file;
}
HANDLE open_inout_handle_write_per(char* inout_address) {
	HANDLE inout_file_wr;
	inout_file_wr = CreateFileA(
		inout_address,
		GENERIC_WRITE, //Open file with write read
		FILE_SHARE_WRITE, //the file should be shared by the threads.
		NULL, //default security mode
		OPEN_ALWAYS, //The files are already created, so files are exist.
		FILE_ATTRIBUTE_NORMAL, //normal attribute
		NULL);//not relevant for open file operations. 
	if (inout_file_wr == INVALID_HANDLE_VALUE) {
		printf("Can't open the file for read within open_inout_handle_write_per func .\n");
		/// neeeeed to do things here
	}
	return inout_file_wr;
}

DWORD get_file_orig_size(HANDLE inout_file) {
	DWORD size = GetFileSize(
		inout_file, NULL
	);
	if (size == INVALID_FILE_SIZE) {
		printf("can't calculate origianl size file. exit\n");
	}
}



static DWORD WINAPI write_output_file_per_thread(LPVOID lpParam) {
	Pthread_relevant_values val = (Pthread_relevant_values)lpParam;
	DWORD return_val = SetFilePointer(
		val->inout_file, //inout file handle
		val->offset, //offset from the start
		NULL, //assume there is no 32 high bits to move
		FILE_BEGIN //offset from the start of the file
	);
	DWORD dwBytesRead = 0, dwBytesWrite = 0;
	int number_to_devide = 0;
	char* str_buffer = (char*)malloc((val->max_length + 1) * sizeof(char));
	if (ReadFile(val->inout_file, str_buffer, val->max_length, &dwBytesRead, NULL)) {
		for (int i = 0; i < dwBytesRead  && str_buffer[i] != '\r' ; i++) {
			if ((str_buffer[i] >= 48 && str_buffer[i] <= 57)) { //check if the char is a number 
				number_to_devide = number_to_devide * 10 + (str_buffer[i] - '0'); //calculate the number related to the mission file
				printf("%c\n", str_buffer[i]);
			}
		}
	}
	else {
		printf("Can't read the file within the write_output_file_per_thread function\n");
		//need to do things ycarmi
	}
	DWORD orig_file_size = get_file_orig_size(val->inout_file); //the size always changed because we write to it
	CloseHandle(val->inout_file);

	printf("%d\n", number_to_devide);
	int array_length;
	int* array_of_div = decompose_into_primary_numbers(number_to_devide, &array_length);
	qsort(array_of_div, array_length, sizeof(int), compare);
	char* output_str = format_output_string(array_of_div,number_to_devide, array_length);
	size_t output_size = strnlen_s(output_str, MAX_OUTPUT_STR_LENGTH);
	HANDLE inout_write_per = open_inout_handle_write_per(val->inout_address);
	return_val = SetFilePointer(
		inout_write_per, //inout file handle
		(int)orig_file_size, //offset from the start
		NULL, //assume there is no 32 high bits to move
		FILE_BEGIN //offset from the start of the file
	);
	if (!WriteFile(inout_write_per, output_str, output_size, &dwBytesWrite, NULL)) {
		printf("error while trying to write into the output file. exit\n");
		//need to do thinggggggs ycarmi
	}
	CloseHandle(inout_write_per);
}
static DWORD create_threads(int max_length, char* inout_address, node* prior_queue, int num_of_threads) {
	Pthread_relevant_values Pthread_values = (Pthread_relevant_values)malloc(sizeof(thread_relevant_values));
	DWORD p_thread_ids;
	if (NULL == Pthread_values)
	{
		printf("can't allocate memory within the create thread function.\n");
		//need to to things ycarmi
	}
	int temp_prior;
	while (prior_queue != NULL)
	{

		Pthread_values->inout_file = open_inout_handle_read_per(inout_address);
	    DWORD orig_file_size = get_file_orig_size(Pthread_values->inout_file);
	    temp_prior = prior_queue->offset_in_bytes; //save the value of the first element
		prior_queue = Pop(prior_queue);  //remove the first element from the queue
		Pthread_values->max_length = max_length;
		Pthread_values->offset = temp_prior;
		Pthread_values->orig_file_size = orig_file_size;
		size_t inout_size = strnlen_s(inout_address, MAX_OUTPUT_STR_LENGTH);
		Pthread_values->inout_address = (char*)malloc((inout_size + 1) * sizeof(char));
		strcpy_s(Pthread_values->inout_address, inout_size + 1, inout_address);
		HANDLE p_thread_handles = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			write_output_file_per_thread,       // thread function name
			Pthread_values,          // argument to thread function 
			0,                      // use default creation flags 
			&p_thread_ids);   // returns the thread identifier
		if (p_thread_handles == NULL) {
			printf("Couldn't create thread number, error code %d\n", GetLastError());
			//NEED TO DO THINGS YCARMI
		}
		DWORD ret_val = WaitForSingleObject(p_thread_handles, MAX_TIME_PER_THREAD);
		if (ret_val != WAIT_OBJECT_0) {
			printf("error occur...\n");
			//need to do things and etc. ycarmi! 
		}
	}
}


//Command line inputs: missions file, priority file, number of missions(equal to number of lines in the input files), number of threads.
int main(int argc, char* argv[])
{
	FILE* fptr_tasks;
	FILE* fptr_priorities;
	node* prior_queue;
	int num_of_tasks = 0, num_of_threads = 0, i = 0, num_of_primary_numbers = 0, number_of_chars_within_tasks_file = 0;
	int temp_prior = 0, temp_task = 0;
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
	int* bytes_per_task = (int*)malloc((num_of_tasks + 2) * sizeof(int));
	if (NULL == bytes_per_task) {
		printf("failed to allocate memory to int array within main. exit\n");
		return ERROR_CODE;
	}
	int max_length = count_bytes_per_task(bytes_per_task, fptr_tasks); //fptr_tasks closed within the function!
	HANDLE inout_file = open_inout_handle_read_per(argv[1]);
	//DWORD orig_file_size = get_file_orig_size(inout_file);
	CloseHandle(inout_file);
	prior_queue = read_priorities_and_create_queue(fptr_priorities);
	create_threads(max_length,argv[1], prior_queue,num_of_threads);
}



