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
    this->localAddress = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
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
