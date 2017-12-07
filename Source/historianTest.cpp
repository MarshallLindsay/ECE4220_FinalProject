/*

historian test program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;
  struct logEntry log;
	log = sock.receiveMessage();
	return(1);
}
