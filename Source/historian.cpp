/*

historian program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;
	char buffer[MSG_SIZE] = "Hello";
	sock.receiveMessage();
	sock.sendMessage(buffer);
	return(1);
}
