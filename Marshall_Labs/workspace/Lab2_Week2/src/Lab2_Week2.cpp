/*
 *
 * Marshall Lindsay
 * ECE 4220
 * Lab2 Week2
 *
 *
 */
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <vector>
#include <time.h>
#include <pthread.h>


using namespace std;

#define MY_PRIORITY (51) //Kernel is 50

void *readThread1(void*);

void* readThread2(void*);

void* saveThread(void*);

string commonBuffer;
vector<string> finalBuffer;

int main(void){


	void** retVal = NULL;

	pthread_t readT1, readT2, saveT;


	pthread_create(&readT1, NULL, readThread1, NULL);
	pthread_create(&saveT, NULL, saveThread, NULL);
	pthread_create(&readT1, NULL, readThread2, NULL);
	pthread_join(readT1, retVal);
	pthread_join(saveT, retVal);
	pthread_join(readT1, retVal);

	for(int i = 0; i < finalBuffer.size(); i++){
		cout<<finalBuffer[i]<<endl;
	}

}

void* readThread1(void* notUsed){
	int* retVal = NULL;
	ifstream file;

	file.open("first.txt");

	if(!file.is_open()){

		cout<<"\nFailed to open the file!"<<endl;

		retVal = (int*)1;
		return((void*)retVal);

	}



	int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //Configures a timer
	if(timer_fd == -1){
		cout<<"\nTimer Creation Failed!"<<endl;
		retVal = (int*)1;
		return((void*)retVal);
	}

	struct itimerspec itval;

	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = 400000;

	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 100000;

	if(timerfd_settime(timer_fd,0,&itval,NULL) == -1){
		cout<<"\nSettime Failed"<<endl;
		retVal = (int*)1;
		return((void*)retVal);
	}

	struct sched_param param;

	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	uint64_t num_periods = 0;


	read(timer_fd, &num_periods, sizeof(num_periods));

	string line;

	while(getline(file, line)){

		commonBuffer = line;
		cout<<"Common Buffer = "<<commonBuffer<<endl;
		read(timer_fd, &num_periods, sizeof(num_periods));
		if(num_periods > 1){
			cout<<"\nMissed window!"<<endl;
			return((void*)retVal);
		}
	}

	retVal = (int*)5;
	pthread_exit((void*)(retVal));
}


void* saveThread(void* notUsed){
	int* retVal = NULL;

	int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //Configures a timer
	if(timer_fd == -1){
		cout<<"\nTimer Creation Failed!"<<endl;
		retVal = (int*)1;
		return((void*)retVal);
	}


	struct itimerspec itval;

	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = 200000;

	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 200000;

	if(timerfd_settime(timer_fd,0,&itval,NULL) == -1){
		cout<<"\nSettime Failed"<<endl;
		retVal = (int*)1;
		return((void*)retVal);
	}

	struct sched_param param;

	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	uint64_t num_periods = 0;

	read(timer_fd, &num_periods, sizeof(num_periods));
	while(commonBuffer.compare("End of file")){
		read(timer_fd, &num_periods, sizeof(num_periods));
		if(num_periods > 1){
			cout<<"\nMissed Window!"<<endl;
			return((void*)retVal);
		}
		cout<<"Adding : "<<commonBuffer<<endl;
		finalBuffer.push_back(commonBuffer);
	}

	return((void*)retVal);

}

void* readThread2(void* notUsed){
	int* retVal = NULL;
	ifstream file;
	file.open("second.txt");
	if(!file.is_open()){
		cout<<"\nFailed to open the file!"<<endl;

		retVal = (int*)1;
		return((void*)retVal);
	}



	int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //Configures a timer
	if(timer_fd == -1){
		cout<<"\nTimer Creation Failed!"<<endl;
		retVal = (int*)1;
		return((void*)retVal);
	}

	struct itimerspec itval;
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = 400000;

	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 300000;
	if(timerfd_settime(timer_fd,0,&itval,NULL) == -1){
		cout<<"\nSettime Failed"<<endl;
		retVal = (int*)1;
		return((void*)retVal);
	}

	struct sched_param param;

	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	uint64_t num_periods = 0;


	read(timer_fd, &num_periods, sizeof(num_periods));
	string line;

	while(getline(file, line)){

		commonBuffer = line;
		cout<<"Common Buffer = "<<commonBuffer<<endl;
		read(timer_fd, &num_periods, sizeof(num_periods));
		if(num_periods > 1){
			cout<<"\nMissed window!"<<endl;
			return((void*)retVal);
		}
	}

	commonBuffer = "End of file";
	retVal = (int*)5;
	pthread_exit((void*)(retVal));
}
