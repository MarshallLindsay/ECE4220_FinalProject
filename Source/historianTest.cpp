/*

historian test program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;
  struct logEntry log;
	log = sock.receiveMessage();
  cout<<log.deviceid<<endl;
	return(1);
}
