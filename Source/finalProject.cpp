/*
Marshall Lindsay
Max Houck
ECE 4220 Final Final Project
*/


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
#include <sys/timerfd.h>
#include "finalProject.h"


//Socket communication methods
SocketCommunication::SocketCommunication(){

  //Create a socket, connectionless
  this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		cout<<"\nSocket creation failed"<<endl;
		exit(1);
	}

  //Set the port.. We could change to dymanic port
  this->portno = 2345;

  //Set the boolval.. Just gotta do it
  this->boolval = 1;

  //Clear all of the server data
  this->length = sizeof(this->server);
  bzero(&this->server, this->length);

  //Set the address family as IPv4
  this->server.sin_family = AF_INET;

  //Set the address to INADDR_ANY
  this->server.sin_addr.s_addr = INADDR_ANY;

  //Set the port number
  this->server.sin_port = htons(this->portno);

  //Bind the socket
  if(bind(this->sockfd, (struct sockaddr *)&this->server, this->length) < 0){
		cout<<"\nFailed to bind the socket!"<<endl;
		exit(1);
	}

  //Set the option for broadcasting
  if (setsockopt(this->sockfd, SOL_SOCKET, SO_BROADCAST, &this->boolval, sizeof(this->boolval)) < 0)
	{
		cout<<"\nError setting socket options"<<endl;
		exit(1);
	}


  //Get the ip of the system
  //Pulled from stackoverflow. Modified slightly for my uses
	//https://stackoverflow.com/questions/579783/how-to-detect-ip-address-change-programmatically-in-linux
  struct ifreq ifr;
  strncpy(ifr.ifr_name, "wlan0", sizeof(ifr.ifr_name));
  if(ioctl(this->sockfd, SIOCGIFADDR, &ifr) >= 0){
    this->localAddress = inet_ntoas(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    cout<<"\nMy IP addr is: "<<this->localAddress<<endl;
  }

}

SocketCommunication::~SocketCommunication(){
  //Close the socket
  close(this->sockfd);
}

int SocketCommunication::sendMessage(char buffer[MSG_SIZE]){
  int n;
  //Copy the message over
  this->broadcast = buffer;

  //Set the broadcast IP
  this->from.sin_addr.s_addr = inet_adr("128.206.19.255");

  //This may throw an error for types.
  n = sendto(this->sockfd, this->broadcast, MSG_SIZE, 0, (struct sockaddr*)&this->from, this->fromlen);

  //Error checking
  if(n < 0){
    return(-1);
  }

  return(1);
}

char* receiveMessage(void){
  int n;
  //Clear the receive character array
  bzero(this->receive, MSG_SIZE);

  n = recvfrom()

}

AnalogInput::AnalogInput() {
  if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
  		printf("wiringPiSPISetup failed\n");
  		exit(0);
  	}
   pthread_t thread1; //create thread pointer
   pthread_create(&thread1, NULL, (void *)&ADCthread, NULL); //execute thread
   //set up the timer to call get_ADC at 1000 Hz
   
   //
}
  
uint16_t AnalogInput::get_ADC() {
  uint8_t spiData[3];
	spiData[0] = 0b00000001; // Contains the Start Bit
	spiData[1] = 0b10000000 | (ADC_CHANNEL << 4);	// Mode and Channel: M0XX0000
												// M = 1 ==> single ended
									// XX: channel selection: 00, 01, 10 or 11
	spiData[2] = 0;	// "Don't care", doesn't matter the value.
	
	// The next function performs a simultaneous write/read transaction over the selected
	// SPI bus. Data that was in the spiData buffer is overwritten by data returned from
	// the SPI bus.
	wiringPiSPIDataRW(SPI_CHANNEL, spiData, 3);
	
	// spiData[1] and spiData[2] now have the result (2 bits and 8 bits, respectively)
	
	this.value =  (3.3/1024)*((spiData[1] << 8) | spiData[2]);
}

void AnalogInput::test_ADC() {
  cout << "ADC value is " << get_ADC() << "\n";
}

void AnalogInput::resetState() {
  this.state = OK;
}

void AnalogInput::ADCthread() {
  //timer and scheduling setup
  int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //intialize realtime scheduling
	 if(timer_fd == -1) {
		 	printf("Error creating timer");
		 	exit(0);
	 }
	 struct itimerspec timerspec;
	 timerspec.it_interval.tv_sec = 0;
	 timerspec.it_interval.tv_nsec = 1000000;
	 timerspec.it_value.tv_sec = 0;
	 timerspec.it_value.tv_nsec = 1000000;
	 if(timerfd_settime(timer_fd,0,&timerspec,NULL) == -1) {
	 	 printf("Error starting timer");
	 	 exit(0);
	 }
	 struct sched_param param;
	 param.sched_priority = 49;
	 sched_setscheduler(0,SCHED_FIFO,&param);
	 uint64_t num_periods = 0;
   
   double last;
   double count;
   this->state = OK;
   
  while(1) {
    while(read(timer_fd, &num_periods,sizeof(num_periods)) == -1); //wait for timer
		if(num_periods >  1) {puts("MISSED WINDOW");exit(1);} //error check
    get_ADC(); //actually get the value over spi
    
    if(this->value == last) //power down status can happen even if overload is already set
      count++;
    if(this->value != last)
      count = 0;
    if(count > ADC_POWERDOWN)
      this->state = POWERDOWN;   
      
    if (this->state == OK) { //overload or underload states cannot occur when power is already down
      if(this->value > ADC_OVERLOAD) //check the value and update status if necessary
        this->state = OVERLOAD;
      if(this->value < ADC_UNDERLOAD)
        this->state = UNDERLOAD;
    }

    last = this->value;
  }
  
  //thread should never exit
  //pthread_exit((void*)retval);
}
