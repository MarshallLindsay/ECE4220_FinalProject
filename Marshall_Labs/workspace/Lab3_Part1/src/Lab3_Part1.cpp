/*
 *
 * Marshall Lindsay
 * ECE 4220
 * Lab3
 *
 *
 */



#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <vector>
#include <wiringPi.h>
#include "ece4220lab3.h"


#define MY_PRIORITY (51) //kernel is 50

//Define IO pins
#define RED (2)
#define YELLOW (3)
#define GREEN (4)
#define BLUE (5)
#define BUTTON (16)

using namespace std;


int main(void){

	//Set up all of the pins as input/output
	//and with pull ups/ pull downs enabled.
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

	//Set the scheduling priority.. Not needed this week?
	struct sched_param param;

	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	//Our polling schedule.
	while(1){

		digitalWrite(RED, HIGH);
		usleep(1000000);
		digitalWrite(RED, LOW);
		digitalWrite(YELLOW, HIGH);
		usleep(1000000);
		digitalWrite(YELLOW, LOW);


		//Only turn the pedestrian light on if the button was pressed.
		if(check_button()){
			digitalWrite(BLUE, HIGH);
			usleep(1000000);
			digitalWrite(BLUE, LOW);
			clear_button();
		}


	}


	return(0);
}
