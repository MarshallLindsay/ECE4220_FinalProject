/*
* test_thrd_mutex.c
* Written by Luis Rivera
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void My_thread1(void *ptr);
int counter = 0;
pthread_mutex_t lock;

int main(void)
{
	pthread_t thread1;
	srand(time(NULL));	// initialize random seed
	
	pthread_create(&thread1, NULL, (void*)&My_thread1, NULL);
	
	while(counter < 1000) {
		usleep(500000+rand()%1000000);	// random time
		pthread_mutex_lock(&lock);
		counter++;
		pthread_mutex_unlock(&lock);
		printf("ParentThread: %d \n", counter);
	}
	
	return(0);
}

void My_thread1(void *ptr)
{
	while(counter < 1000) {
		usleep(500000+rand()%1000000);	// random time
		pthread_mutex_lock(&lock);
		counter++;
		pthread_mutex_unlock(&lock);
		printf("My_thread1: %d \n", counter);
	}
	
	pthread_exit(0);
}
