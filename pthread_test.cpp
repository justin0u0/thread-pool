/**
 * @author justin0u0<mail@justin0u0.com>
 * @copyright Copyright (c) 2022
 */

#include <stdio.h>
#include <unistd.h>

#include "pthread/thread_pool.h"

/**
 * Should print out 4 numbers every 2 seconds and should terminate when
 * about 20 tasks done with the followings configurations.
 */

#define THREAD_POOL_THREADS 4
#define THREAD_POOL_BUFFER_SIZE 20
#define NUM_TASKS 40
#define SLEEP_SECONDS 2

// sleepPrint sleeps for SLEEP_SECONDS and print the value passed in by the argument
void* sleepPrint(void* arg) {
	int value = *(int*)arg;

	sleep(SLEEP_SECONDS);

	printf("%d\n", value);

	return nullptr;
}

int main() {
	ThreadPool* pool = new ThreadPool(THREAD_POOL_THREADS, THREAD_POOL_BUFFER_SIZE);

	pool->start();

	int* arr = new int[NUM_TASKS];
	for (int i = 0; i < NUM_TASKS; ++i) {
		arr[i] = i;
		pool->addTask(new ThreadPoolTask(&sleepPrint, (void*)&arr[i]));
	}

	pool->terminate();
	pool->join();

	delete[] arr;
	delete pool;

	return 0;
}
