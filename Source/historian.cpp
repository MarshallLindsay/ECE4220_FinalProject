/*

historian program

*/


#include "finalProject.h"

void* readMessages(void*);
SocketCommunication sock;

int main(void){


	pthread_t messageReader;
	void** retVal;
	pthread_create(&messageReader, NULL, readMessages, NULL);
	char buffer[MSG_SIZE];
	string message = "This is a message";
	struct logEntry log;
	gettimeofday(&log.timestamp,NULL);
  log.deviceid = 15;
  log.digin1state = 1;
  log.digin2state = 0;
  log.digin3state = 1;
  log.digout1state = 0;
  log.digout2state = 1;
  log.digout3state = 1;
  log.analoginstate = 1;
  log.analogvalue = 1.21;
	log.note = "This is a note";

	cout<<log.deviceid<<endl;
	sock.sendMessage(log);
	sock.sendMessage(message);

	pthread_join(messageReader, retVal);
	return(1);
}

void* readMessages(void* ptr){
	//Set thread to real time
	struct sched_param param;
	param.sched_priority = 51;
	sched_setscheduler(0,SCHED_FIFO,&param);

	while(1){
		sock.receiveMessage();
	}
}
