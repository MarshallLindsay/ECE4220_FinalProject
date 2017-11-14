/*
* test_thrd_sem1.c
* Written by Luis Rivera
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>

#define INIT_VALUE	1

void My_thread1(void *ptr);
int counter = 0;
sem_t my_semaphore;	// counting semaphore

int main(void)
{
	pthread_t thread1;
	srand(time(NULL));	// initialize random seed

	// What happens when using different initial values?
	sem_init(&my_semaphore, 0, INIT_VALUE);
	
	pthread_create(&thread1, NULL, (void*)&My_thread1, NULL);
	
	while(counter < 20) {
		usleep(500000+rand()%1000000);	// random time
		sem_wait(&my_semaphore);
		counter++;
		sem_post(&my_semaphore);
		printf("ParentThread: %d \n", counter);
	}
	
	pthread_join(thread1, NULL);		// wait for thread to be done.
	
	return(0);
}

void My_thread1(void *ptr)
{
	while(counter < 20) {
		usleep(500000+rand()%1000000);	// random time
		sem_wait(&my_semaphore);
		counter++;
		sem_post(&my_semaphore);
		printf("My_thread1: %d \n", counter);
	}
	
	pthread_exit(0);
}
