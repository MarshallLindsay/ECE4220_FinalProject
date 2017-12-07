/*

historian test program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;

	//Set thread to real time
	struct sched_param param;
	param.sched_priority = 51;
	sched_setscheduler(0,SCHED_FIFO,&param);

	while(1){
		sock.receiveMessage();
	}
	return(1);
}
