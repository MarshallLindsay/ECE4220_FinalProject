/*
  Marshall Lindsay
  Max Houck
  ECE 4220 Final Project

*/
//#define RTU YES //include this if you are compiling for the RTU. comment this out if you are compiling on a workstation for the historian
#define HISTORIAN YES

#ifndef FINALPROJECT_H
#define FINALPROJECT_H

using namespace std;

#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <time.h>
#include <sstream>
#include <semaphore.h>
#include <thread>
#ifdef RTU //workstations don't have these so compiler wilvim l fail. see note above.
	#include <wiringPi.h>
	#include <wiringPiSPI.h>
#endif
#include <pthread.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <vector>

#define CHAR_DEV "/dev/RTU"

#define HSEND_RREC_PORT (2345) //historian send _ rtu receive port
#define RSEND_HREC_PORT (2346) //rtu send _ historian receive port
#define MSG_SIZE (100)

//definitions for ADC
#define SPI_CHANNEL	      0		 // 0 or 1
#define SPI_SPEED 	2000000		 // Max speed is 3.6 MHz when VDD = 5 V
#define ADC_CHANNEL       1		 // Between 1 and 3
#define ADC_REF 3.3 					 //reference voltage for ADC
#define ADC_OVERLOAD 2.2 			 //overload voltage
#define ADC_UNDERLOAD 0.8 		 //underload voltage
#define ADC_POWERDOWN 25			 //number of consecutive equal measurements pbefore power line is considered down
#define ADC_TOLERANCE 0.1		   //tolerance used for powerdown condition counting
#define OK 1 									 //adc states
#define OVERLOAD 2
#define UNDERLOAD 3
#define POWERDOWN 4
#define ACK1 5
#define ACK2 6
#define ACK3 7

//wiringPi pins
#define DIGIN1PIN 26
#define DIGIN2PIN 23
#define DIGIN3PIN 28
#define SEVENENABLE 29
#define SEVENA 5
#define SEVENB 6
#define SEVENC 25
#define SEVEND 2

struct logEntry {
  int analoginstate,digin1state,digin2state,digin3state,digout1state,digout2state,digout3state;
  double analogvalue;
  timeval timestamp;
  int deviceid;
  string note; //note what actually triggered the event
};

#ifdef RTU
class DigitalOutput{
private:
	int outputNumber;
	int cdev_id;
  int pinNumber;  //What is the pin number?
  int value;     //What is the state of the input...has an event occurred?
  bool eventFlag;
public:
  DigitalOutput(int,int);         //Initialize the input hardware parameters
  void setValue(int);    //Set the state if an event happened, or clear if the event is over.
  int getValue(void);
  void resetFlag();
  bool getEvent();
};

class DigitalInput{
private:
  int pinNumber;  //pin number using wiringpi scheme
  int value;     //What is the value on the pin?
  bool eventFlag;
public:
  DigitalInput(int);      //Initialize the input hardware parameters.
  int getValue();      //Get the value of the pin.
  void update();
  void resetFlag();
  bool getEvent();
};

class AnalogInput{
private:
  void get_ADC();
  int state;     //What is the state of the input? Has an event occurred.
  double value;   //What is the value on the pin?
  double last;
  double count;
  bool eventFlag;
public:
  AnalogInput(); //default constructor, will use ADC channel 1
  void test_ADC();
  ~AnalogInput();       //Destructor.
  void setState(bool);  //Set the state if an event happened, or clear if the event is over.
  double getValue();     //Get the value on the pin.
  int getState();       //Get the state of the pin.
  void update();
  void resetFlag();
  bool getEvent();
};

class _7seg{
	private:
	public:
		_7seg();
		~_7seg();
		void init();
		void setValue(int);

};
#endif

class SocketCommunication{
private:
  int sockfd;
  int portno;
  int boolval;
  socklen_t fromlen;
  int length;
  string localAddress;
  char broadcast[MSG_SIZE];
  char recMessage[MSG_SIZE];
  struct logEntry receive;
  struct sockaddr_in serveraddress;
  struct sockaddr_in fromaddress;
  struct hostent *server;
public:
  SocketCommunication();
  SocketCommunication(int port);
  ~SocketCommunication();
  int sendMessage(struct logEntry buffer);
  int sendMessage(vector<struct logEntry> buffer);
  int sendMessage(string buffer);
  char* receiveMessage(void);
};

class CharacterCommunication{
private:
  int cdev_id;
  int dummy;
  char broadcast[MSG_SIZE];
  char receive[MSG_SIZE];
public:
  CharacterCommunication();
  ~CharacterCommunication();
  int openDevice(); //Maybe overload to allow different devices
  int writeToDevice(char buffer[MSG_SIZE]);
  char* readFromDevice(void); //Char pointer return?
};

#endif
