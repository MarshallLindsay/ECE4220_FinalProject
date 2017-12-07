/*

historian program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;
	char buffer[MSG_SIZE] = "Hello";
	int n = sock.sendMessage(buffer);
	return(1);
}
