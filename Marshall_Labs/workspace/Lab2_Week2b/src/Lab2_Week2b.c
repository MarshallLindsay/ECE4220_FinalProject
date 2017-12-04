/*
 *
 * Marshall Lindsay
 * ECE 4220
 * Lab2 Week2
 *
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <pthread.h>


#define MY_PRIORITY (49) //Kernel is 50

void *readThread1(void* buffer);

void* readThread2(void* buffer);

void* saveThread(void* finalBuffer);

char* commonBuffer;
char* finalBuffer;

int main(void){


	void** retVal;

	pthread_t readT1, readT2, saveT;


	pthread_create(&readT1, NULL, readThread1, (void*)commonBuffer);


	pthread_join(readT1, retVal);

}

void* readThread1(void* buffer){



	int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //Configures a timer
	if(timer_fd == -1){
		printf("\nTimer creation Failed!\n");
		return(1);
	}

	struct itimerspec itval;

	itval.it_interval.tv_sec = 5;
	itval.it_interval.tv_nsec = 0;

	itval.it_value.tv_sec = 1;
	itval.it_value.tv_nsec = 0;

	if(timerfd_settime(timer_fd,0,&itval,NULL) == -1){

		printf("\nSettime Failed!\n");
		return(1);
	}

	struct sched_param param;

	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	uint64_t num_periods = 0;


	read(timer_fd, &num_periods, sizeof(num_periods));

	int i;
	for(i = 0; i< 10; i++){
		printf("\n%d", i);
		read(timer_fd, &num_periods, sizeof(num_periods));
	}


	pthread_exit((void*)(buffer));
}


void* readThread2(void* buffer){


}

void* saveThread(void* finalBuffer){


}
