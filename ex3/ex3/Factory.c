#include "factory.h"


int compare(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

int* decompose_into_primary_numbers(int number, int* num_of_primary_numbers) {
	int* primary_numbers_local = (int*)malloc(2 * (ceil(sqrt(number)) + 1) * sizeof(int)); //Upper bond for number of divisidor of n is n^0.5 * 2 , we take a little bit more space for it for small n
	//math proof for that can be find here https://math.stackexchange.com/questions/1053062/upper-limit-for-the-divisor-function
	int i = 3, p = 0;
	while ((number % 2) == 0) {
		number = number / 2;
		primary_numbers_local[p] = 2;
		p++;
	}
	while (i <= sqrt(number)) {
		while ((number % i) == 0) {
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
queue_pointer* read_priorities_and_create_queue(FILE* fptr) {
	queue_pointer* pq = InitializeQueue();
	pq->pq_head_node = NULL;
	char task[MAX_TASK_LEN];
	char c;
	int i = 0, curr_offset;
	for (c = getc(fptr); c != EOF; c = getc(fptr)) {
		task[i] = c;
		if (c == '\n') {
			task[i] = '\0';
			curr_offset = atoi(task);
				pq->pq_head_node = Push(pq, curr_offset);
			i = 0;
		}
		else
			i++;
	}
	return pq;
}

int calculate_max_line( FILE* fptr) {
	int char_count = 0;
	int max_length = 1;
	int index = 0;
	char c;
	index++;
	for (c = getc(fptr); c != EOF; c = getc(fptr)) {
		char_count++;
		if (c == '\n') {
			char_count++; // \n is 2 chars in read_file function
			if (max_length < char_count)
				max_length = char_count;
			index++;
		}
	}
	fclose(fptr);
	return max_length;
}

HANDLE open_inout_handle_read_write(char* inout_address) {
	HANDLE inout_file;
	inout_file = CreateFileA(
		inout_address,
		GENERIC_READ | GENERIC_WRITE, //Open file with write read
		FILE_SHARE_READ | FILE_SHARE_WRITE, //the file should be shared by the threads.
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

DWORD get_file_orig_size(HANDLE inout_file) {
	DWORD size = GetFileSize(
		inout_file, NULL
	);
	if (size == INVALID_FILE_SIZE) {
		printf("can't calculate origianl size file. exit\n");
	}
}

// pop first mission, read the relevant lines from the current place, call, synchronzie  
static DWORD WINAPI write_output_file_per_thread(LPVOID lpParam) {
	Pthread_relevant_values val = (Pthread_relevant_values)lpParam;
	int offset = 0, number_to_devide = 0,last_offset = 0;
	DWORD dwBytesRead = 0, dwBytesWrite = 0,return_val = 0;
	//Every thread will execute tasks from the input file until the queue will be empty. 
	//Pop from queue must be within the critical regions, as well as write.
	while (!Empty(val->p_q_head)) {
		number_to_devide = 0;
		read_lock(val->lock_t);
		printf("the queue before pop");
		PrintQueue(val->p_q_head);
		last_offset = -1*offset;
		offset = Top(val->p_q_head);
		Pop(val->p_q_head); // the first element within the queue step forward in all threads. current is previues first element.
		printf("the queue after pop, offset = %d",offset);
		PrintQueue(val->p_q_head);
			return_val = SetFilePointer(
				val->inout_file, //inout file handle
				offset, //offset from the start
				NULL, //assume there is no 32 high bits to move
				FILE_BEGIN //offset from the start of the file
			);

		if (return_val == INVALID_SET_FILE_POINTER) {
			printf("cannot set file pointer within write ouput file per thread function. exit\n");
			//not sure it is enopgh to close!
			DestroyQueue(val->p_q_head);
			free(val);
			exit(1);
			// end of comment
		}

		char* str_buffer = (char*)malloc((val->max_length + 1) * sizeof(char));
		if (ReadFile(val->inout_file, str_buffer, val->max_length, &dwBytesRead, NULL)) {
			str_buffer[dwBytesRead] = '\0';
			printf("str - %s", str_buffer);
			for (int i = 0; i < dwBytesRead && str_buffer[i] != '\r'; i++) {
				if ((str_buffer[i] >= 48 && str_buffer[i] <= 57)) { //check if the char is a number 
					number_to_devide = number_to_devide * 10 + (str_buffer[i] - '0'); //calculate the number related to the mission file
				}
			}
			
		}
		else {
			printf("Can't read the file within the write_output_file_per_thread function\n");
			//not sure it is enopgh to close!
			DestroyQueue(val->p_q_head);
			free(val);
			exit(1);
			// end of comment
		}
		free(str_buffer);
		read_release(val->lock_t);

		int array_length;
		int* array_of_div = decompose_into_primary_numbers(number_to_devide, &array_length);
		qsort(array_of_div, array_length, sizeof(int), compare);
		char* output_str = format_output_string(array_of_div, number_to_devide, array_length);
		size_t output_size = strnlen_s(output_str, MAX_OUTPUT_STR_LENGTH);
		write_lock(val->lock_t);
		DWORD orig_file_size = get_file_orig_size(val->inout_file); //the size always changed because we write to it, so this line in the safe place
		return_val = SetFilePointer(
			val->inout_file, //inout file handle
			(int)orig_file_size, //offset from the start
			NULL, //assume there is no 32 high bits to move
			FILE_BEGIN //offset from the start of the file
		);
		if (!WriteFile(val->inout_file , output_str, output_size, &dwBytesWrite, NULL)) {
			printf("error while trying to write into the output file. exit\n");
			//not sure it is enopgh to close!
			DestroyQueue(val->p_q_head);
			free(val);
			exit(1);
			// end of comment
		}
		write_release(val->lock_t);
		free(array_of_div); 
		free(output_str);
	}
	CloseHandle(val->inout_file);
}

void kill_the_program_and_die(lock* p_lock, lock* prior_lock, Pthread_relevant_values* Pthread_values, queue_pointer* pq /*TBD ADD EVERYTHING*/)
{
	//ALL THE CLOSE SHOULD BE HERE!!!! 
	DestroyLock(p_lock);
	DestroyLock(prior_lock);
}

static DWORD create_threads(int max_length, queue_pointer* q_head, int num_of_threads) {
	lock* p_lock = InitializLock(num_of_threads); //initialze the lock
	lock* priority_lock = InitializLock(num_of_threads);
	Pthread_relevant_values Pthread_values[MAX_THREADS]; //array of thread_relevant_values for the createthread function
	HANDLE p_thread_handles[MAX_THREADS];	//handles to threads
	DWORD p_thread_ids[MAX_THREADS];	//handels to threads ids
	for (int i = 0; i < num_of_threads; i++) {
		Pthread_values[i] = (Pthread_relevant_values)malloc(sizeof(thread_relevant_values));//allocate memory for thread_values[i]
		Pthread_values[i]->p_q_head = q_head; //same queue for all the threads.
		Pthread_values[i]->inout_file =open_inout_handle_read_write(inout_file_address); //same file name for everythread
		Pthread_values[i]->max_length = max_length;
		Pthread_values[i]->lock_t = p_lock;
		Pthread_values[i]->priority_lock = priority_lock;
		p_thread_handles[i] = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			write_output_file_per_thread,       // thread function name
			Pthread_values[i],          // argument to thread function 
			0,                      // use default creation flags 
			&p_thread_ids[i]);   // returns the thread identifier
		if (p_thread_handles[i] == NULL) {
			printf("Couldn't create thread number, error code %d\n", GetLastError());
			//not sure if it is everything!!!!
			for (int j = 0; j < i; j++) {
				CloseHandle(Pthread_values[i]->inout_file);
				CloseHandle(Pthread_values[i]);
				DestroyLock(p_lock);
				DestroyLock(priority_lock);
			}
			exit(1);
			//end of comment 
		}
	}
	DWORD wait_code = WaitForMultipleObjects(num_of_threads, p_thread_handles, TRUE, TIMEOUT);
	if (wait_code != WAIT_OBJECT_0) {
	}
	for (int i = 0; i < num_of_threads; i++) {
		DWORD error_status = CloseHandle(p_thread_handles[i]);
		if (error_status == 0)
		{
			printf("close handle failed! continue to try close other handles if exist\n");
		}
		free(Pthread_values[i]);
	}
	DestroyLock(p_lock);
	
}

void Initialize_global_file_path(char* inout_file_address, char* file_path) {

	if (strcpy_s(inout_file_address, MAX_OUTPUT_STR_LENGTH, file_path)) {
		printf("failed to copy the file path, exit\n ");
		exit(1);
	}

}


//Command line inputs: missions file, priority file, number of missions(equal to number of lines in the input files), number of threads.
int main(int argc, char* argv[])
{
	FILE* fptr_tasks = NULL;
	FILE* fptr_priorities = NULL;
	int num_of_tasks = 0, num_of_threads = 0, i = 0, num_of_primary_numbers = 0, number_of_chars_within_tasks_file = 0;
	int temp_prior = 0, temp_task = 0;
	char temp_char = argv[3][0];

	if (argc != 5) {
		printf("Command line number of inputs incorrect. exit\n");
		exit(1);
	}

	Initialize_global_file_path(inout_file_address, argv[1]);
	int is_open_tasks = fopen_s(&fptr_tasks, inout_file_address, "r");
	if (NULL == fptr_tasks || is_open_tasks != 0) {
		printf("Can't open the tasks input file \n");
		exit(1);
	}
	int is_open_priorities = fopen_s(&fptr_priorities, argv[2], "r");
	if (NULL == fptr_priorities || is_open_priorities != 0) {
		printf("Can't open the priorities input file \n");
		fclose(fptr_tasks);
		exit(1);
	}
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
	if (num_of_threads <= 0 || num_of_tasks <= 0) {
		printf("Number of threads or number of tasks is zero or negative nubmer, which breaks the rules of the program. exit\n");
		fclose(fptr_priorities);
		fclose(fptr_tasks);
		exit(1);
	}
	int max_length = calculate_max_line(fptr_tasks); //fptr_tasks closed within the function.
	queue_pointer* p_q = read_priorities_and_create_queue(fptr_priorities);
	fclose(fptr_priorities); //Priorities are already witihn the queue.
	create_threads(max_length, p_q, num_of_threads);
}