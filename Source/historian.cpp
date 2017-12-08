/*

historian program

*/


#include "finalProject.h"



void printWelcomeMessage();
int handleUserInput(string input);
void startRTUS();
void sendCommand();
void printHistory();
void createLogEntry(char* message);

char broadcast[MSG_SIZE];
timeval systemStartTime;
sem_t sendBroadcast_semaphore;
vector<struct logEntry> history;

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
	//Gotta do it twice cause.. maths
	sem_post(&sendBroadcast_semaphore);

}

void sendCommand(){
	//Broadcast the command
}

void printHistory(){
	//Print the history
	for(int i = 0; i < history.size() - 1; i++){
		cout<<"------------------------------------------------"<<endl;
		cout<<"Cause of event : "<<history[i].note<<endl;
		cout<<"Event from : "<<history[i].deviceid<<endl;
		cout<<"Time of event from start: "<<history[i].timestamp.tv_sec<<"(S) "<<history[i].timestamp.tv_usec<<"(uS)"<<endl;
		cout<<"Analog state: "<<history[i].analoginstate;
		cout<<"Analog value: "<<history[i].analogvalue;
		cout<<"Digital Input 1 State: "<<history[i].digin1state<<endl;
		cout<<"Digital Input 2 State: "<<history[i].digin2state<<endl;
		cout<<"Digital Input 3 State: "<<history[i].digin3state<<endl;
		cout<<"Digital Output 1 State: "<<history[i].digout1state<<endl;
		cout<<"Digital Output 2 State: "<<history[i].digout2state<<endl;
		cout<<"Digital Output 3 State: "<<history[i].digout3state<<endl;
		cout<<"------------------------------------------------"<<endl;
	}
}

void* sendMessages(void* ptr){
	//Initialize a socket on the send port
	SocketCommunication sock(HSEND_RREC_PORT);
	string message;
	while(1){
		sem_wait(&sendBroadcast_semaphore);
		message = broadcast;
		sock.sendMessage(message);
	}
}

void* readMessages(void* ptr){
	//Initialize a socket on the read port
	SocketCommunication sock(RSEND_HREC_PORT);
	//Variable to hold the message recieved
	char buffer[MSG_SIZE] = "Initialize";
	//Clear the buffer variable

	while(1){
		//Get a message and copy to buffer
		bzero(buffer, MSG_SIZE);
		strcpy(buffer, sock.receiveMessage());
		//Filter messages
		if(buffer[1] == ',' ){
			//Create a log entry of the message
			//cout<<"About to do the log thing : "<<buffer<<endl;
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

	pos = message.find(delimiter);
	state = message.substr(0, pos);
	//cout<<state<<endl;
	stringstream(state) >> entry.analoginstate;
	message.erase(0, pos + delimiter.length());
	pos = message.find(delimiter);
	state = message.substr(0, pos);
	//cout<<state<<endl;
	stringstream(state) >> entry.digin1state;
	message.erase(0, pos + delimiter.length());
	pos = message.find(delimiter);
	state = message.substr(0, pos);
	//cout<<state<<endl;
	stringstream(state) >> entry.digin2state;
	message.erase(0, pos + delimiter.length());
	pos = message.find(delimiter);
	state = message.substr(0, pos);
	//cout<<state<<endl;
	stringstream(state) >> entry.digin3state;
	message.erase(0, pos + delimiter.length());
	pos = message.find(delimiter);
	state = message.substr(0, pos);
	//cout<<state<<endl;
	stringstream(state) >> entry.digout1state;
	message.erase(0, pos + delimiter.length());
	pos = message.find(delimiter);
	state = message.substr(0, pos);
	//cout<<state<<endl;
	stringstream(state) >> entry.digout2state;
	message.erase(0, pos + delimiter.length());
	pos = message.find(delimiter);
	state = message.substr(0, pos);
	//cout<<state<<endl;
	stringstream(state) >> entry.digout3state;
	message.erase(0, pos + delimiter.length());

	size_t sz;
	pos = message.find(delimiter);
	analogValueString = message.substr(0, pos);
	//cout<<analogValueString<<endl;
	entry.analogvalue = stod(analogValueString,&sz);
	message.erase(0, pos+ delimiter.length());

	pos = message.find(delimiter);
	timeSecString = message.substr(0, pos);
	//cout<<timeSecString<<endl;
	entry.timestamp.tv_sec = stod(timeSecString, &sz) + systemStartTime.tv_sec;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	timeMicroSecString = message.substr(0, pos);
	//cout<<timeMicroSecString<<endl;
	entry.timestamp.tv_usec = stod(timeSecString, &sz) + systemStartTime.tv_usec;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	deviceNumberString = message.substr(0, pos);
	//cout<<deviceNumberString<<endl;
	stringstream(deviceNumberString) >> entry.deviceid;
	message.erase(0, pos + delimiter.length());

	pos = message.find(delimiter);
	note = message.substr(0, pos);
	//cout<<note<<endl;
	entry.note = note;
	message.erase(0, pos + delimiter.length());

	history.push_back(entry);

}
