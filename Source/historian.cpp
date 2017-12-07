/*

historian program

*/


#include "finalProject.h"



void printWelcomeMessage();
int handleUserInput(string input);
void startRTUS();
void sendCommand();
void printHistory();

char broadcast[MSG_SIZE];
timeval systemStartTime;
sem_t sendBroadcast_semaphore;

void* readMessages(void*);
void* sendMessages(void*);

int main(void){
	string userInput;
	int option;
	pthread_t sender, reader;
	//Initialize the sendBroadcast_semaphore
	sem_init(&sendBroadcast_semaphore, 0, 0);
	pthread_create(&sender, NULL, sendMessages, NULL);
	pthread_create(&reader, NULL, readMessages, NULL);
	while(1){
		//print the welcome message
		printWelcomeMessage();
		//get user input
		getline(cin,userInput);
		//Handle user input
		handleUserInput(userInput);
	}


	return(1);
}

void printWelcomeMessage(){
	cout<<"\nWelcome to the Historian Program!"<<endl;
	cout<<"\nPlease select an option:"<<endl;
	cout<<"1)Start the RTUs"<<endl;
	cout<<"2)Send a command to the RTUs"<<endl;
	cout<<"3)Print the event history since start"<<endl;
	cout<<"4)Exit"<<endl;
}

int handleUserInput(string input){
	if(input == "1"){
		startRTUS();
		return(1);
	}else if(input == "2"){
		sendCommand();
		return(1);
	}else if(input == "3"){
		printHistory();
		return(1);
	}else if(input == "4"){
		exit(1);
	}else{
		cout<<"Invalid Input"<<endl;
		return(0);
	}
}

void startRTUS(){
	//Broadcast the start message
	int n;
	//Clear the broadcast buffer
	bzero(broadcast, MSG_SIZE);
	//Set the message
	strcpy(broadcast, "#start");
	//Get a start time
	gettimeofday(&systemStartTime, NULL);
	//Post to the sendBroadcast_semaphore, indicating a message should be sent.
	sem_post(&sendBroadcast_semaphore);

}

void sendCommand(){
	//Broadcast the command
}

void printHistory(){
	//Print the history
}

void* sendMessages(void* ptr){
	//Initialize a socket on the send port
	SocketCommunication sock(HSEND_RREC_PORT);
}

void* readMessages(void* ptr){
	//Initialize a socket on the read port
	SocketCommunication sock(RSEND_HREC_PORT);
	char buffer[MSG_SIZE];

	while(1){
		strcpy(buffer, sock.receiveMessage());
		cout<<buffer<<endl;
	}
}
