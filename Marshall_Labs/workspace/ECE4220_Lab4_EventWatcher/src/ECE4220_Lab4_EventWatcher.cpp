/*
 * Marshall Lindsay
 * ECE4220 Lab4
 * Event Watcher
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPi.h>
#include <time.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sched.h>
#include "ece4220lab3.h"

using namespace std;

#define PRIORITY (51)
#define BUTTON (16)

int main(void){
	//Time stuff
	struct timespec event_time;
	int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	if(timer_fd == -1){
		cout<<"\nFailed to create the timer"<<endl;
		exit(-1);
	}

	struct itimerspec itval;
	//TEST - CHANGE THIS VALUE TO MUCH LARGER *****************************
	itval.it_interval.tv_nsec = 75000000; //75 ms
	itval.it_interval.tv_sec =  0;
	itval.it_value.tv_sec = 1;
	itval.it_value.tv_nsec = 0;
	if(timerfd_settime(timer_fd, 0, &itval, NULL) == -1){
		cout<<"\nSettime Failed"<<endl;
		exit(-1);
	}

	//IO
	wiringPiSetupGpio();
	pullUpDnControl(BUTTON, PUD_DOWN);

	//Pipe creation
	int pipe_eventWrite;

	if((pipe_eventWrite = open("/home/pi/workspace/ECE4220_Lab4_Main/Debug/EventWatcher", O_WRONLY)) < 0){
		cout<<"\nPipe EventWatcher Failed"<<endl;
		exit(-1);
	}

	//Scheduling
	struct sched_param param;
	param.sched_priority = PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;

	//Loop
	read(timer_fd, &num_periods, sizeof(num_periods));
	clear_button();
	while(1){
		//cout<<"\nLoop"<<endl;
		if(check_button()){
			clear_button();
			clock_gettime(CLOCK_REALTIME, &event_time);
			//TEST - Check this time against the one sent to the main program
			cout<<"\n"<<(int)(event_time.tv_sec);

			if(write(pipe_eventWrite, &event_time, sizeof(event_time)) != sizeof(event_time)){
				cout<<"\nEventWatcher pipe write error"<<endl;
				exit(-1);
			}

			read(timer_fd, &num_periods, sizeof(num_periods));
			cout<<"\n"<<num_periods<<endl;;


		}
	}

}
