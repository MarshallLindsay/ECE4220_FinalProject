/*

historian program

*/


#include "finalProject.h"

int main(void){

	SocketCommunication sock;
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
	return(1);
}
