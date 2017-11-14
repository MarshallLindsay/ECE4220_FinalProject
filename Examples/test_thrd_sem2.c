/*
* test_thrd_sem2.c
* Written by Luis Rivera
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>

#define INIT_VALUE	1
#define THR_COUNT	5

void My_thread1(void *ptr);
sem_t my_semaphore;

int main(int argc, char *argv[])
{
	int i, initial_val = INIT_VALUE;
	if(argc == 2){
		initial_val = atoi(argv[1]);	// user input for initial sem value
	}
		
	pthread_t threads[THR_COUNT];
	int thr_index[THR_COUNT];	
	sem_init(&my_semaphore, 0, initial_val);	// initialize semaphore
	
	for(i = 0; i < THR_COUNT; i++) {
		thr_index[i] = i+1;
		pthread_create(&threads[i], NULL, (void*)&My_thread1, (void*)(thr_index+i));
	}
	
	for(i = 0; i < THR_COUNT; i++) {
		pthread_join(threads[i], NULL);		// wait for threads to be done.
	}
	
	return(0);
}

void My_thread1(void *ptr)
{
	int *index = (int *)ptr;
	int value;
	
	sem_wait(&my_semaphore);
	sem_getvalue(&my_semaphore, &value);
	printf("Thread #: %d, sem value: %d\n", *index, value);
	fflush(stdout);
	sleep(1);
	sem_post(&my_semaphore);
		
	pthread_exit(0);
}
