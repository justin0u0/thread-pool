/**
 * @author justin0u0<mail@justin0u0.com>
 * @copyright Copyright (c) 2022
 */

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>

class ThreadPool;

class ThreadPoolTask {
	friend class ThreadPool;
private:
	void* (*f)(void*);
	void* arg;
public:
	ThreadPoolTask(void* (*f)(void*), void* arg) : f(f), arg(arg) {}
};


class ThreadPool {
private:
	// number of threads in the pool
	int numThreads;

	// threads
	pthread_t* threads;

	// pthread lock and conditional variables
	pthread_mutex_t lock;
	pthread_cond_t addCond;
	pthread_cond_t removeCond;

	// ring buffer
	size_t bufferSize;
	int head;
	int tail;
	size_t size;
	ThreadPoolTask** buffer;

	// signal for gracefully shutdown
	bool terminating;

	// removeTask removes a task from the buffer and returns the task
	ThreadPoolTask* removeTask() {
		pthread_mutex_lock(&lock);

		while (!size && !terminating) {
			// sleep until add task notify
			pthread_cond_wait(&removeCond, &lock);
		}

		if (terminating) {
			pthread_mutex_unlock(&lock);
			return nullptr;
		}

		ThreadPoolTask* task = buffer[head];
		head = (head + 1) % bufferSize;
		--size;

		pthread_cond_signal(&addCond);
		pthread_mutex_unlock(&lock);

		return task;
	}

	// run runs a single thread's work
	static void* run(void* arg) {
		ThreadPool* pool = (ThreadPool*)arg;

		while (!pool->terminating) {
			ThreadPoolTask* task = pool->removeTask();

			if (task != nullptr) {
				// do the task works
				(*(task->f))(task->arg);
	
				// delete the task instance after finishing task
				// note that the argument should be deleted in other place
				delete task;
			}
		}

		return nullptr;
	}
public:
	ThreadPool(int numThreads, size_t bufferSize) : numThreads(numThreads), bufferSize(bufferSize) {
		threads = new pthread_t[numThreads];

		buffer = new ThreadPoolTask*[bufferSize];
		head = 0;
		tail = 0;
		size = 0;

		terminating = false;

		pthread_mutex_init(&lock, 0);
		pthread_cond_init(&addCond, 0);
		pthread_cond_init(&removeCond, 0);
	}

	~ThreadPool() {
		pthread_cond_destroy(&removeCond);
		pthread_cond_destroy(&addCond);
		pthread_mutex_destroy(&lock);

		delete[] buffer;
		delete[] threads;
	}

	// addTask adds task into thread pool
	void addTask(ThreadPoolTask* task) {
		pthread_mutex_lock(&lock);

		while (size >= bufferSize && !terminating) {
			// sleep until remove task notify
			pthread_cond_wait(&addCond, &lock);
		}

		if (terminating) {
			pthread_mutex_unlock(&lock);
			return;
		}

		buffer[tail] = task;
		tail = (tail + 1) % bufferSize;
		++size;

		pthread_cond_signal(&removeCond);
		pthread_mutex_unlock(&lock);
	}

	// start starts all thread pool threads
	void start() {
		for (int i = 0; i < numThreads; ++i) {
			pthread_create(&threads[i], 0, ThreadPool::run, (void*)this);
		}
	}

	// join waits for all thread pool threads to be terminated
	void join() {
		for (int i = 0; i < numThreads; ++i) {
			pthread_join(threads[i], 0);
		}
	}

	// terminate signals thread pool to start termination
	void terminate() {
		pthread_mutex_lock(&lock);

		terminating = true;
		pthread_cond_broadcast(&addCond);
		pthread_cond_broadcast(&removeCond);

		pthread_mutex_unlock(&lock);
	}
};

#endif // _THREAD_POOL_H_
