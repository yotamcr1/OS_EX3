#include "factory.h"


//the function create all the mutex and semaphore for the struct and initialize the lock struct
//input: number of threads
//output: initialized struct of lock

lock* InitializLock(int num_of_threads) {
	HANDLE mutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // don't lock mutex immediately
		NULL);				//unnamed mutex
	if (mutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		exit(1);
	}
	HANDLE turnstile = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // don't lock mutex immediately
		NULL);				//unnamed mutex
	if (turnstile == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		exit(1);
	}
	HANDLE roomEmpty = CreateSemaphore(
		NULL,	// Default security attributes 
		1,		// Initial Count 
		num_of_threads,		// Maximum Count 
		NULL); // unnamed semaphore
	if (roomEmpty == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
		exit(1);
	}
	lock* new_lock = allocate_place_for_lock(mutex, turnstile, roomEmpty);
	return new_lock;
}


//the function allocate place for the lock and fill his fields
//input: handles to 2 mutex and 1 semaphore
//output: initialized lock struct

lock* allocate_place_for_lock(HANDLE mutex, HANDLE turnstile, HANDLE roomEmpty) {
	lock* new_lock = (lock*)malloc(sizeof(lock));
	if (new_lock != NULL) {
		new_lock->Mutex = mutex;
		new_lock->roomEmpty = roomEmpty;
		new_lock->turnstile = turnstile;
		new_lock->activeReaders = 0;
	}
	return new_lock;
}


//The function performs a lock for reading
//threads can read at the same time but while another thread is writing.
//input: pointer to lock struct

void read_lock(lock* lock) {
	DWORD wait_code;
	BOOL ret_val;
	wait_code = WaitForSingleObject(lock->turnstile, TIMEOUT);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting for turnstile\n");
		return ERROR_CODE;
	}
	ret_val = ReleaseMutex(lock->turnstile);
	if (FALSE == ret_val)
	{
		printf("Error when releasing turnstile\n");
		return ERROR_CODE;
	}
	wait_code = WaitForSingleObject(lock->Mutex, TIMEOUT);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting for mutex\n");
		return ERROR_CODE;
	}
	lock->activeReaders = lock->activeReaders + 1;
	if (lock->activeReaders == 1) {
		wait_code = WaitForSingleObject(lock->roomEmpty, TIMEOUT);
		if (WAIT_OBJECT_0 != wait_code)
		{
			printf("Error when waiting for roomEmpty\n");
			return ERROR_CODE;
		}
	}
	ret_val = ReleaseMutex(lock->Mutex);
	if (FALSE == ret_val)
	{
		printf("Error when releasing mutex\n");
		return ERROR_CODE;
	}
	return;
}


//The function performs a release of the lock from a reading lock made by the same thread
//input: pointer to lock struct

void read_release(lock* lock) {
	DWORD wait_code;
	BOOL ret_val;
	LONG previous_count;// have to check if initialize here its OK
	wait_code = WaitForSingleObject(lock->Mutex, TIMEOUT);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting for mutex\n");
		return ERROR_CODE;
	}
	lock->activeReaders = lock->activeReaders - 1;
	if (lock->activeReaders = 0) {
		ret_val = ReleaseSemaphore(lock->roomEmpty, 1, &previous_count);
		if (FALSE == ret_val)
		{
			printf("Error when releasing roomEmpty\n");
			return ERROR_CODE;
		}
	}
	ret_val = ReleaseMutex(lock->Mutex);
	if (FALSE == ret_val)
	{
		printf("Error when releasing mutex\n");
		return ERROR_CODE;
	}
	return;
}


//The function performs a lock for writing
//input: pointer to lock struct

void write_lock(lock* lock) {
	DWORD wait_code;
	wait_code = WaitForSingleObject(lock->turnstile, TIMEOUT);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting for turnstile\n");
		return ERROR_CODE;
	}
	wait_code = WaitForSingleObject(lock->roomEmpty, TIMEOUT);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting for roomEmpty\n");
		return ERROR_CODE;
	}
	return;
}

//The function performs a release of the lock from a writing lock made by the same thread
//input: pointer to struct lock

void write_release(lock* lock) {
	BOOL ret_val;
	LONG previous_count;
	ret_val = ReleaseMutex(lock->turnstile);
	if (FALSE == ret_val)
	{
		printf("Error when releasing turnstile\n");
		return ERROR_CODE;
	}
	ret_val = ReleaseSemaphore(lock->roomEmpty, 1, &previous_count);
	if (FALSE == ret_val)
	{
		printf("Error when releasing roomEmpty\n");
		return ERROR_CODE;
	}
}

//the function release all the resources of the lock
//input: pointer to struct lock

void DestroyLock(lock* lock) {
	CloseHandle(lock->Mutex);
	CloseHandle(lock->roomEmpty);
	CloseHandle(lock->turnstile);
	free(lock);
	lock = NULL;
}






/*maybe replace all the errors with:
static void ReportErrorAndEndProgram()
{
	printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError() );
	exit(1);
}
*/