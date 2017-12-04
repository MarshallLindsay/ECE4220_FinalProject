/*
 *
 * Marshall Lindsay
 * ECE4220
 * Lab3 Part2
 *
 *
 */

#include <iostream>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <vector>
#include <semaphore.h>
#include <wiringPi.h>
#include "ece4220lab3.h"

using namespace std;

//Define values for IO devices
#define RED (2)
#define YELLOW (3)
#define GREEN (4)
#define BLUE (5)
#define BUTTON (16)

#define INIT_VALUE (1)

//Counting semaphore
sem_t my_semaphore;


void* redLight(void*);
void* yellowLight(void*);
void* pedLight(void*);

int redPriority = 0;
int yellowPriority = 0;
int pedPriority = 0;

int main(int argc, char **argv){
	if(argc > 1){
		redPriority = atoi(argv[1]);
		yellowPriority = atoi(argv[2]);
		pedPriority = atoi(argv[3]);
	}else{
		redPriority = 51;
		yellowPriority = 51;
		pedPriority = 51;
	}
	//Set up our i/o
	wiringPiSetupGpio();
	pinMode(RED, OUTPUT);
	pinMode(YELLOW, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BLUE, OUTPUT);
	pinMode(BUTTON, INPUT);

	pullUpDnControl(RED, PUD_DOWN);
	pullUpDnControl(YELLOW, PUD_DOWN);
	pullUpDnControl(GREEN, PUD_DOWN);
	pullUpDnControl(BLUE, PUD_DOWN);
	pullUpDnControl(BUTTON, PUD_DOWN);

	//Initialize semaphore with the INIT_VALUE
	sem_init(&my_semaphore, 0, INIT_VALUE);

	pthread_t redThread, yellowThread, pedThread;

	//Create threads for each of the lights.
	pthread_create(&redThread, NULL, redLight, NULL);
	pthread_create(&yellowThread, NULL, yellowLight, NULL);
	pthread_create(&pedThread, NULL, pedLight, NULL);

	//Wait for red to finish. This is just to keep the threads
	//running. At no point will red finish.
	pthread_join(redThread, NULL);



	return(0);
}


void* redLight(void*){
	//Initialize scheduling parameters structure
	struct sched_param param;

	//Set the red priority
	param.sched_priority = redPriority;
	sched_setscheduler(0, SCHED_FIFO, &param);
	while(1){
		//Wait for the semaphore to be released
		sem_wait(&my_semaphore);
		//Turn off the yellow and blue lights
		digitalWrite(YELLOW, LOW);
		digitalWrite(BLUE, LOW);
		//Turn on the red light
		digitalWrite(RED, HIGH);
		//Wait for a second
		usleep(1000000);
		//Release the semaphore
		sem_post(&my_semaphore);
		//Delay to allow proper scheduling.
		//Could mess with the delay and make shorter, but this works with
		//without any problems.
		usleep(100);
	}
}


void* yellowLight(void*){
	//Initialize scheduling parameters structure.
	struct sched_param param;
	//Set the priority to the yellow priority.
	param.sched_priority = yellowPriority;
	sched_setscheduler(0, SCHED_FIFO, &param);

	while(1){
		//Wait for the semaphore to be released.
		sem_wait(&my_semaphore);
		//Turn off the red and blue lights.
		digitalWrite(RED, LOW);
		digitalWrite(BLUE, LOW);
		//Turn on the yellow light.
		digitalWrite(YELLOW, HIGH);
		//Delay for a second.
		usleep(1000000);
		//Release the semaphore.
		sem_post(&my_semaphore);
		//Scheduling delay.
		usleep(100);
	}
}

void* pedLight(void*){
	//Initialize the scheduling parameters structure.
	struct sched_param param;
	//Set the priority to the pedestrian priority.
	param.sched_priority = pedPriority;
	sched_setscheduler(0, SCHED_FIFO, &param);
	while(1){
		//Check button is from the module we installed.
		//If the button is pressed, it will hold a TRUE value until
		//clearButton() is called.
		//Wait for the button press.
		if(check_button()){
			//Wait for the semaphore to be released.
			sem_wait(&my_semaphore);
			//Turn off the red and yellow lights.
			digitalWrite(RED, LOW);
			digitalWrite(YELLOW, LOW);
			//Turn on the blue light.
			digitalWrite(BLUE, HIGH);
			//Delay for one second.
			usleep(1000000);
			//Release the semaphore.
			sem_post(&my_semaphore);
			//Scheduling delay.
			usleep(100);
			//Clear the button value.
			clear_button();
		}

	}
}
