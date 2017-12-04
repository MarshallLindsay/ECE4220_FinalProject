/*
 *
 * Marshall Lindsay
 * ECE4220
 * Lab1
 *
 *
 */


#include <iostream>
#include <wiringPi.h>

using namespace std;

void ledFlashing(void);
void clearLeds(void);
void platinumRecord(void);
void lab1Setup(void);

#define GREEN 8
#define YELLOW 9
#define PB1 0
#define PB2 2
#define PB3 3
#define PB4 14
#define PB5 13
#define SPEAKER 12

int main(void){

	wiringPiSetup();

	lab1Setup();

	clearLeds();

	ledFlashing();

	platinumRecord();


	return(0);
}

void lab1Setup(void){
	pinMode(GREEN, OUTPUT);
	pinMode(YELLOW, OUTPUT);
	pinMode(PB1, INPUT);
	pinMode(PB2, INPUT);
	pinMode(PB3, INPUT);
	pinMode(PB4, INPUT);
	pinMode(PB5, INPUT);
	pinMode(SPEAKER, OUTPUT);

	pullUpDnControl(GREEN, PUD_DOWN);
	pullUpDnControl(YELLOW, PUD_DOWN);
	pullUpDnControl(PB1, PUD_UP);
	pullUpDnControl(PB2, PUD_UP);
	pullUpDnControl(PB3, PUD_UP);
	pullUpDnControl(PB4, PUD_UP);
	pullUpDnControl(PB5, PUD_UP);
	pullUpDnControl(SPEAKER, PUD_DOWN);


}


void ledFlashing(void){


	digitalWrite(GREEN, LOW);
	digitalWrite(YELLOW, LOW);

	int i;

	for(i = 0; i < 10; i++){
		if(i%2){

			digitalWrite(GREEN, HIGH);
			digitalWrite(YELLOW, LOW);
			delay(1000);
		}else{

			digitalWrite(GREEN, LOW);
			digitalWrite(YELLOW, HIGH);
			delay(1000);
		}
	}

	digitalWrite(GREEN, LOW);
	digitalWrite(YELLOW, LOW);

}

void clearLeds(void){
	digitalWrite(GREEN, LOW);
	digitalWrite(YELLOW, LOW);
}

void platinumRecord(void){

	int x, pb;
	while(1){
		cout<<"Please enter a push button number: "<<endl;
		cin>>pb;
		if(pb > 5 || pb < 1){
			cout<<"Please enter a valid push buttom number from 1 to 5"<<endl;
		}else{
			break;
		}
	}

	switch(pb){
	case 1:
		pb = 0;
		break;
	case 2:
		pb = 2;
		break;
	case 3:
		pb = 3;
		break;
	case 4:
		pb = 14;
		break;
	case 5:
		pb = 13;
		break;

	}

	while(1){
		x = digitalRead(pb);
		//cout<<x<<endl;
		if(x == 0){
			digitalWrite(SPEAKER, HIGH);
			delay(10);
			digitalWrite(SPEAKER, LOW);
			delay(10);
		}
	}


}
