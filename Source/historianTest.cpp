/*

historian test program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;
	char buffer[MSG_SIZE];
	//Set thread to real time
	struct sched_param param;
	param.sched_priority = 51;
	sched_setscheduler(0,SCHED_FIFO,&param);

	while(1){
		strcpy(buffer, sock.receiveMessage());
		cout<<buffer<<endl;
	}
	return(1);
}
