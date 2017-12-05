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

#define CHAR_DEV "/dev/MarshallMaxFinal"
#define MSG_SIZE (50)

//definitions for ADC
#define SPI_CHANNEL	      0	// 0 or 1
#define SPI_SPEED 	2000000	// Max speed is 3.6 MHz when VDD = 5 V
#define ADC_CHANNEL       1	// Between 1 and 3
#define ADC_REF 3.3 //reference voltage for ADC

using namespace std;

class DigitalInput{
private:
  int pinNumber;  //What is the pin number?
  bool state;     //What is the state of the input...has an event occurred?
  bool value;     //What is the value on the pin?


public:

  DigitalInput();         //Initialize the input hardware parameters
  ~DigitalInput();        //Destructor
  void setState(bool);    //Set the state if an event happened, or clear if the event is over.
  void setPullUp();       //Set the pullup for the pin.
  void setPullDown();     //Set the pulldown for the pin.
  void setAsInterrupt();  //Set the pin as an interrupt.
  bool getValue();        //Get the value on the pin.
  bool getState();        //Get the state of the pin.
};

class DigitalOutput{
private:
  int pinNumber;  //What is the pin number?
  bool state;     //What is the state on the pin?
  bool value;     //What is the value on the pin?

public:
  DigitalOutput();      //Initialize the input hardware parameters.
  ~DigitalOutput(); /    //Destructor.
  void setPullUp();     //Set the pullup for the pin.
  void setPullDown();   //Set the pulldown for the pin.
  void setState(bool);  //Set the state if an event happened, or clear if the event is over.
  bool getState();      //Get the state of the pin.
  bool getValue();      //Get the value of the pin.
  void setValue(bool);  //Set the value of the pin.
};

class AnalogInput{
private:
  uint16_t get_ADC();
  bool state;     //What is the state of the input? Has an event occurred.
  double value;   //What is the value on the pin?
  void ADCthread();
public:
  AnalogInput(); //default constructor, will use ADC channel 1
  void test_ADC();
  ~AnalogInput();       //Destructor.
  void setState(bool);  //Set the state if an event happened, or clear if the event is over.
  double getValue();     //Get the value on the pin.
  bool getState();       //Get the state of the pin.
  bool resetState();    //reset state flag when it is logged
}

class SocketCommunication{
private:
  int sockfd;
  int portno;
  int boolval;
  socklen_t fromlen;
  int length;
  string localAddress;
  char broadcast[MSG_SIZE];
  char receive[MSG_SIZE];
  struct sockaddr_in server;
  struct sockaddr_in from;
  struct hostent *server;
public:
  SocketCommunication();
  ~SocketCommunication();
  int sendMessage(char buffer[MSG_SIZE]);
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
