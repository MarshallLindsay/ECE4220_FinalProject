/*
  Marshall Lindsay
  Max Houck
  ECE 4220 Final Project

*/
#ifndef FINALPROJECT_H
#define FINALPROJECT_H

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
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <pthread.h>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <vector>


#define CHAR_DEV "/dev/MarshallMaxFinal" ///pretty sure we can delete this

using namespace std;

struct logEntry {
  int analoginstate,digin1state,digin2state,digin3state,digout1state,digout2state,digout3state;
  double analogvalue;
  timeval timestamp;
  int deviceid;
  string note; //note what actually triggered the event
};
#define MSG_SIZE (100)

//definitions for ADC
#define SPI_CHANNEL	      0	// 0 or 1
#define SPI_SPEED 	2000000	// Max speed is 3.6 MHz when VDD = 5 V
#define ADC_CHANNEL       2	// Between 1 and 3
#define ADC_REF 3.3 		//reference voltage for ADC
#define ADC_OVERLOAD 2.1 	//overload voltage
#define ADC_UNDERLOAD 0.9 	//underload voltage
#define ADC_POWERDOWN 10	//number of consecutive equal measurements pbefore power line is considered down

#define OK 1
#define OVERLOAD -1
#define UNDERLOAD -2
#define POWERDOWN -3


class DigitalOutput{
private:
  int pinNumber;  //What is the pin number?
  int value;     //What is the state of the input...has an event occurred?
  bool eventFlag;
public:
  DigitalOutput(int);         //Initialize the input hardware parameters
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

class SocketCommunication{
private:
  int sockfd;
  int portno;
  int boolval;
  socklen_t fromlen;
  int length;
  string localAddress;

  char broadcast[MSG_SIZE];
  struct logEntry receive;

  struct sockaddr_in serveraddress;
  struct sockaddr_in fromaddress;
  struct hostent *server;
public:
  SocketCommunication();
  ~SocketCommunication();
  int sendMessage(logEntry buffer);
  int sendMessage(string buffer);
  char* receiveMessage(void);
};

#endif
