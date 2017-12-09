/*

historian program

*/

#include "finalProject.h"

void printWelcomeMessage();
int handleUserInput(string input);
void startRTUS();
void sendCommand();
void printHistory();
void printSimpleHistory();
void printSuperSimpleHistory();
vector<struct logEntry> sortHistory();
void createLogEntry(char* message);

//Global variable to hold a message to be broadcast
char broadcast[MSG_SIZE];
//Semaphore to indicate a message should be broadcasted
sem_t sendBroadcast_semaphore;
//Mutex to protect the history vector
pthread_mutex_t historyMutex;
//History vector. Contains all of the logs since starting
vector<struct logEntry> history;

//Pthread functions
void* readMessages(void*);
void* sendMessages(void*);

int main(void){
	//String to hold the user input
	string userInput;
	int option;
	pthread_t sender, reader;
	//Initialize the sendBroadcast_semaphore
	sem_init(&sendBroadcast_semaphore, 0, 0);
	//Start the pthreads
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
	cout<<"4)Print simplified event history"<<endl;
	cout<<"5)Print super simplified envent history"<<endl;
	cout<<"6)Exit"<<endl;
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
		printSimpleHistory();
		return(1);
	}else if(input == "5"){
		printSuperSimpleHistory();
		return(1);
	}else if(input == "6"){
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
	//Post to the sendBroadcast_semaphore, indicating a message should be sent.
	sem_post(&sendBroadcast_semaphore);
	//Gotta do it twice cause.. Weird things
	sem_post(&sendBroadcast_semaphore);

}

void sendCommand(){
	//Broadcast the command
	//Clear the broadcast buffer
	bzero(broadcast, MSG_SIZE);
	//Get the command from the user
	string command;
	getline(cin, command);
	//Copy the command from the user to the broadcast buffer
	strcpy(broadcast, command.c_str());
	//Post to the broadcast semaphore
	sem_post(&sendBroadcast_semaphore);

}

void printHistory(){
	//sort the history
	vector<struct logEntry> log;
	log = sortHistory();
	//Print the history
	if(log.empty()){
		cout<<"------------------------------------------------"<<endl;
		cout<<"No events have been logged!"<<endl;
		cout<<"------------------------------------------------"<<endl;
	}else{
		for(unsigned int i = 0; i < log.size() - 1; i++){
			cout<<"------------------------------------------------"<<endl;
			cout<<"Cause of event : "<<log[i].note<<endl;
			cout<<"Event from : "<<log[i].deviceid<<endl;
			cout<<"Time of event from start: "<<log[i].timestamp.tv_sec<<"(S) "<<log[i].timestamp.tv_usec<<"(uS)"<<endl;
			cout<<"Analog state: "<<log[i].analoginstate<<endl;
			cout<<"Analog value: "<<log[i].analogvalue<<endl;
			cout<<"Digital Input 1 State: "<<log[i].digin1state<<endl;
			cout<<"Digital Input 2 State: "<<log[i].digin2state<<endl;
			cout<<"Digital Input 3 State: "<<log[i].digin3state<<endl;
			cout<<"Digital Output 1 State: "<<log[i].digout1state<<endl;
			cout<<"Digital Output 2 State: "<<log[i].digout2state<<endl;
			cout<<"Digital Output 3 State: "<<log[i].digout3state<<endl;
			cout<<"------------------------------------------------"<<endl;
		}
	}
}

void printSimpleHistory(){
	//Sort the history
	vector<struct logEntry> log;
	log = sortHistory();
	//Print the history
	if(log.empty()){
		cout<<"------------------------------------------------"<<endl;
		cout<<"No events have been logged!"<<endl;
		cout<<"------------------------------------------------"<<endl;
	}else{
		for(unsigned int i = 0; i < log.size() - 1; i++){
			cout<<"------------------------------------------------"<<endl;
			cout<<"Cause of event : "<<log[i].note<<endl;
			cout<<"Event from : "<<log[i].deviceid<<endl;
			cout<<"Time of event from start: "<<log[i].timestamp.tv_sec<<"(S) "<<log[i].timestamp.tv_usec<<"(uS)"<<endl;
			cout<<"------------------------------------------------"<<endl;
		}
	}

}


void printSuperSimpleHistory(){
	//sort the history
	vector<struct logEntry> log;
	log = sortHistory();
	//Print the history
	if(log.empty()){
		cout<<"No events have been logged!"<<endl;
	}else{
		for(unsigned int i = 0; i < log.size() - 1; i++){
			cout<<"Event from: "<<log[i].deviceid<<" at " <<log[i].timestamp.tv_sec<<"."<<log[i].timestamp.tv_usec<<" is "<<log[i].note<<endl;
		}
	}
}

void* sendMessages(void* ptr){
	//Initialize a socket on the send port
	SocketCommunication sock(HSEND_RREC_PORT);
	string message;
	while(1){
		//Wait for the broadcast semaphore
		sem_wait(&sendBroadcast_semaphore);
		//Copy the message
		message = broadcast;
		//Send the message
		sock.sendMessage(message);
	}
}

void* readMessages(void* ptr){
	//Initialize a socket on the read port
	SocketCommunication sock(RSEND_HREC_PORT);
	//Variable to hold the message recieved
	char buffer[MSG_SIZE];
	//Clear the buffer variable

	while(1){
		//Get a message and copy to buffer
		//Clear the local buffer
		bzero(buffer, MSG_SIZE);
		//Receive a message and copy to the local buffer
		strcpy(buffer, sock.receiveMessage());
		//Filter messages
		if(buffer[1] == ',' ){
			//Create a log entry of the message
			createLogEntry(buffer);
		}else{
			cout<<"Master message received"<<endl;
		}
	}
}

void createLogEntry(char* buffer){
	//Take the message in buffer, create a log, push to the log vector
	struct logEntry entry;
	string message = buffer;
	string delimiter = ",";

	string state;
	string analogValueString;
	double analogValue;
	string timeSecString;
	string timeMicroSecString;
	string deviceNumberString;
	int deviceNumber;
	string note;
	size_t pos = 0;

	//Parse the message and put contents into appropriate variables
	pos = message.find(delimiter);
	state = message.substr(0, pos);
	stringstream(state) >> entry.analoginstate;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	state = message.substr(0, pos);
	stringstream(state) >> entry.digin1state;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	state = message.substr(0, pos);
	stringstream(state) >> entry.digin2state;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	state = message.substr(0, pos);
	stringstream(state) >> entry.digin3state;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	state = message.substr(0, pos);
	stringstream(state) >> entry.digout1state;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	state = message.substr(0, pos);
	stringstream(state) >> entry.digout2state;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	state = message.substr(0, pos);
	stringstream(state) >> entry.digout3state;
	message.erase(0, pos + delimiter.length());

	size_t sz;
	pos = message.find(delimiter);
	analogValueString = message.substr(0, pos);
	entry.analogvalue = stod(analogValueString,&sz);
	message.erase(0, pos+ delimiter.length());

	pos = message.find(delimiter);
	timeSecString = message.substr(0, pos);
	entry.timestamp.tv_sec = stod(timeSecString, &sz);
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	timeMicroSecString = message.substr(0, pos);
	entry.timestamp.tv_usec = stod(timeMicroSecString, &sz);
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	deviceNumberString = message.substr(0, pos);
	stringstream(deviceNumberString) >> entry.deviceid;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	note = message.substr(0, pos);
	entry.note = note;
	message.erase(0, pos + delimiter.length());

	//Add the log to the history buffer
	//Lock the history mutex
	pthread_mutex_lock(&historyMutex);
	//Add the log to the global history
	history.push_back(entry);
	//Unlock the history mutex
	pthread_mutex_unlock(&historyMutex);

}

vector<struct logEntry> sortHistory(){
	//Create a local copy of the global history,
	//sort it, and then return the sorted version.

	//Lock the history mutex.
	pthread_mutex_lock(&historyMutex);
	//Make a local copy of the global history
	vector<struct logEntry> sortedHistory = history;
	//Unlock the histor mutex
	pthread_mutex_unlock(&historyMutex);

	struct logEntry temp;

	//Selection sort based off the times
	int minLocation = 0;
	for(int j = 0; j < sortedHistory.size()-1; j++){
		minLocation = j;
	for(int i = j; i < sortedHistory.size()-1; i++){

			if(sortedHistory[i].timestamp.tv_sec < sortedHistory[minLocation].timestamp.tv_sec){
				minLocation = i;
			}else if(sortedHistory[i].timestamp.tv_sec == sortedHistory[minLocation].timestamp.tv_sec){
				if(sortedHistory[i].timestamp.tv_usec < sortedHistory[minLocation].timestamp.tv_usec){
					minLocation = i;
				}
			}
		}
		temp = sortedHistory[j];
		sortedHistory[j] = sortedHistory[minLocation];
		sortedHistory[minLocation] = temp;
	}

	return(sortedHistory);
}
