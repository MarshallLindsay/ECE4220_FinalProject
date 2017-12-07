/*

historian program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;
	char buffer[MSG_SIZE] = "Hello";
	sock.receiveMessage();

	return(1);
}
