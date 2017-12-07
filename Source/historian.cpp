/*

historian program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;
	int deviceid = 15;
	struct logEntry log;
	gettimeofday(&log.timestamp,NULL);
  log.deviceid = deviceid;
  log.digin1state = 1;
  log.digin2state = 0;
  log.digin3state = 1;
  log.digout1state = 0;
  log.digout2state = 1;
  log.digout3state = 1;
  log.analoginstate = 0;
  log.analogvalue = 1.21;

	cout<<log.deviceid<<endl;
	sock.sendMessage(log);
	log = sock.receiveMessage();
	return(1);
}
