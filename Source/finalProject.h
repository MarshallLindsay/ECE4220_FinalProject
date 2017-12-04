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

#define CHAR_DEV "/dev/MarshallMaxFinal"
#define MSG_SIZE (50)

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


class AnalogInput{
private:
  int pinNumber;  //What is the pin number?
  bool state;     //What is the state of the input? Has an event occurred.
  double value;   //What is the value on the pin?

public:
  AnalogInput();        //Initialize the input hardware parameters.
  ~AnalogInput();       //Destructor.
  void setState(bool);  //Set the state if an event happened, or clear if the event is over.
  void setPullUp();      //Set the pullup for the pin.
  void setPullDown();    //Set the pulldown for the pin.
  double getValue();     //Get the value on the pin.
  bool getState();       //Get the state of the pin.
  void setValue();       //Set the value of the pin.
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

class SocketCommunication{
private:
  int sockfd;
  int portno;
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
