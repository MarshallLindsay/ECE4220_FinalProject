//============================================================================
// Name        : ECE4220_Lab5.cpp
// Author      : Marshall Lindsay
// Version     :
// Copyright   :
// Description : ECE4220 Lab5 code
//============================================================================

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


#define CHAR_DEV "/dev/MarshallLab6"
#define MSG_SIZE (40)
using namespace std;
bool masterOrCommander = false;

struct station{

  string theIP;
  int theRandom;
};



int processMessage(char*);

void writeToCharacterDevice(char buffer[MSG_SIZE]);

station me;
station master;

int main(int argc, char *argv[]) {

	//Check for command line errors
	if(argc != 2){
		cout<<"\nIncorrect number of command line arguments!"<<endl;
		return(-1);
	}

	srand(time(NULL));
	char charDevMessage[MSG_SIZE];
	int sock;
	int length;
	int boolval = 1;
	char buff[MSG_SIZE];
	int n;


	//Create a socket, connectionless
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		cout<<"\nSocket creation failed"<<endl;
		return(-1);
	}

	//Put together the structure to hold all of the communication information
	struct sockaddr_in server;
	struct sockaddr_in from;
	length = sizeof(server);

	//Clear all of the data.
	bzero(&server,length);
	//Set the address family IPv4
	server.sin_family = AF_INET;
	//Set the address to INADDR_ANY.. It will be set later with bind()
	server.sin_addr.s_addr = INADDR_ANY;
	//Set the port from the command line argument
	server.sin_port = htons(atoi(argv[1]));


	//Bind the socket to the port and host IP
	if(bind(sock, (struct sockaddr *)&server, length) < 0){
		cout<<"\nFailed to bind the socket!"<<endl;
		return(-1);
	}


	//Set the option for broadcasting.
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
	{
		cout<<"\nError setting socket options"<<endl;
		return(-1);
	}

	//Obtain the ip address of the server
	//Pulled from stackoverflow. Modified slightly for my uses
	//https://stackoverflow.com/questions/579783/how-to-detect-ip-address-change-programmatically-in-linux

	struct ifreq ifr;
	string myIp;
	strncpy(ifr.ifr_name, "wlan0", sizeof(ifr.ifr_name));
	if(ioctl(sock, SIOCGIFADDR, &ifr) >= 0){
		myIp = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
		cout<<"\nMy IP addr is: "<<myIp<<endl;
	}

	me.theIP = myIp;

	//Set up the some info for the structure to receive messages in.
	socklen_t fromlen;
	fromlen = sizeof(struct sockaddr_in);
	string message;
	string send;

	//Create a thread to monitor the character device for button presses


	while (1)
	{
		//Clear the buffer
		bzero(buff, MSG_SIZE);

		//Check if master
			//Check if a button was pressed

		//Receive message from Hermes. The address of the sender is saved in the from structure.
		n = recvfrom(sock, buff, MSG_SIZE, 0, (struct sockaddr*)&from, &fromlen);

		if(n < 0){
			cout<<"\nError recvfrom"<<endl;
		}

		//Prompt that I got a message
		cout<<"\nYou've got mail!"<<endl;
		//Print the message for debugging.
		cout<<buff<<endl;
		//Convert the buffer to a "# " + myIp + convertedstring
		message = buff;
		message = message.substr(0, message.size() -1);

		if(message == "WHOIS"){
			cout<<"\nStarting the WHOIS procedure"<<endl;
			if(masterOrCommander == true){
				send = "Marshall on " + myIp + " is master!";
				const char * sending = send.c_str();
				//Send a message to the client indicating that I am the master
				n = sendto(sock, sending, MSG_SIZE, 0, (struct sockaddr *)&from, fromlen);
				//Error Check
				if(n < 0){
					cout<<"\nError WHOIS sendto"<<endl;
					return(-1);
				}
			}
		}else if(message == "VOTE"){
			cout<<"\nStarting the VOTE procedure"<<endl;
			master.theIP = "";
			master.theRandom = 0;
			//Grab a rando number between 1 and 10
			int number = (rand() % 10) + 1;
			me.theRandom = number;
			//Convert to a string
			string converted;
			stringstream ss;
			ss<<number;
			converted = ss.str();
			//Set the broadcast ip
			from.sin_addr.s_addr = inet_addr("128.206.19.255"); //broadcast address
			//Send the message
			send = "# " + myIp + " " + converted;
			const char * sending = send.c_str();
			cout<<"\nSending : "<<send<<endl;
			n = sendto(sock, sending, MSG_SIZE, 0, (struct sockaddr*)&from, fromlen);
			//Error Check
			if(n < 0){
				cout<<"\nError VOTE sento"<<endl;
				return(-1);
			}
		}else if(message = "@A"){
			//Write to the kernel space
			charDevMessage = "@A";
			writeToCharacterDevice(charDevMessage);

			//If master, broadcast the note
		}else if(message = "@B"){
			//Write tot he kernel space
			charDevMessage = "@B";
			writeToCharacterDevice(charDevMessage);

			//If master, broadcast the note
		}else if(message = "@C"){
			//Write to the kernel space
			charDevMessage = "@C";
			writeToCharacterDevice(charDevMessage);

			//If master, broadcast the note
		}else if(message = "@D"){
			//Write to the kernel space
			charDevMessage = "@D";
			writeToCharacterDevice(charDevMessage);

			//If master, broadcast the note
		}else if(message = "@E"){
			//Write to the kernel space
			charDevMessage = "@E";
			writeToCharacterDevice(charDevMessage);

			//If master, broadcast the note
		}else{
			if(processMessage(buff) != 0){
				cout<<"\nError processing message!"<<endl;
				return(-1);
			}
		}

	 }

	return(0);
}

//Need to do all of the string parsing here. Return 0 for success. Anything else for fail.
int processMessage(char* buffer){
	int random;
	//Convert to a sting for better parsing.
	string message = buffer;
	//Atomize the string
	//Example altered from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
	//There might be an elegant way to do this in a loop, but a hammer and brute force will work too.
	string delimiter = " ";
	string hashtag;
	string supposedIP;


	size_t pos = 0;
	pos = message.find(delimiter);
    hashtag = message.substr(0, pos);
    cout << hashtag << endl;
    message.erase(0, pos + delimiter.length());
	pos = message.find(delimiter);
	supposedIP = message.substr(0,pos);
	cout << supposedIP << endl;
	message.erase(0, pos + delimiter.length());
	cout << message << endl;

	stringstream ss(message);
	ss >> random;

	//Need to compare the strings and check for validity.
	if(hashtag != "#"){
		cout<<"Not a valid message!"<<endl;
		return(0);
	}

	if(random > master.theRandom){
	    master.theRandom = random;
	    master.theIP = supposedIP;
	}else if(random < master.theRandom){
	    cout<<"\nThe master's credentials are greater"<<endl;
	  }else{
	    if(supposedIP.size() > master.theIP.size()){
	      master.theRandom = random;
	      master.theIP = supposedIP;
	    }else if(supposedIP.size() < master.theIP.size()){
	      cout<<"\nThe master's credentials are greater"<<endl;
	    }else if(supposedIP > master.theIP){
	      master.theIP = supposedIP;
	      master.theRandom = random;
	    }else{
	      cout<<"\nThe master's credentials are greater!"<<endl;
	    }
	  }


	    if(master.theIP == me.theIP){
	    	cout<<"\nI am the master!"<<endl;
	    	masterOrCommander = true;
	    	return(0);
	    }else{
	    	cout<<"\nI am not the master!"<<endl;
	    	masterOrCommander = false;
	    	return(0);
	    }
	//Need to determine if I was beaten

	return(1);
}


//Function to write to the character device.
void writeToCharacterDevice(char buffer[MSG_SIZE]){
	int cdev_id, dummy;
	// Open the Character Device for writing
	if((cdev_id = open(CHAR_DEV, O_WRONLY)) == -1) {
		cout<<"Cannot open device "<<CHAR_DEV<<endl;
		exit(1);
	}
	dummy = write(cdev_id, buffer, sizeof(buffer));
	if(dummy != sizeof(buffer)) {
		cout<<"Write failed, leaving...\n"<<endl;
		break;
	}

}
