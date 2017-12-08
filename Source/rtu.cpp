/*
RTU program
*/
#include "finalProject.h"

//function definitions
void* ADCthread(void*);
void* DiginThread(void* ptr);
void* NetworkSendThread(void* ptr);
void* NetworkReceiveThread(void* prt);
void ISR1(); //ISRn controls digital inputs
void ISR2();
void ISR3();
struct logEntry gather_log(DigitalInput* digin1, DigitalInput* digin2, DigitalInput* digin3,
      DigitalOutput* digout1, DigitalOutput* digout2, DigitalOutput* digout3, AnalogInput* analoginput);

//class and variable definitions
DigitalInput digin1(DIGIN1PIN);
DigitalInput digin2(DIGIN2PIN);
DigitalInput digin3(DIGIN3PIN);
DigitalOutput digout1(9,1);
DigitalOutput digout2(7,2);
DigitalOutput digout3(21,3);
AnalogInput analoginput;
_7seg seven;
vector<struct logEntry> log;
struct timeval zerotime;
int deviceid;
SocketCommunication receiveSock(HSEND_RREC_PORT);

int main(int argc, char **argv) {
    if(argc != 2 || *argv[1] > '9' || *argv[1] < '0') { //check inputs and set device id
      cout << "Usage: rtu <deviceid>\r\nDevice ID must be 1 digit";
      exit(0);
    }
    deviceid = *argv[1] - '0'; //this is sneaky
    receiveSock.receiveMessage(); //receive 1 dummy message to start up receive sockets
    gettimeofday(&zerotime,NULL); //set zero time
    wiringPiSetup();
    int dummy1 = wiringPiISR(DIGIN1PIN,INT_EDGE_BOTH,&ISR1); //initialize interrupts for the inputs
    int dummy2 = wiringPiISR(DIGIN2PIN,INT_EDGE_BOTH,&ISR2);
    int dummy3 = wiringPiISR(DIGIN3PIN,INT_EDGE_BOTH,&ISR3);
    pthread_t thread1,thread5,thread6; //create thread pointers
  	pthread_create(&thread1, NULL, ADCthread, (void*)&analoginput); //execute thread that will time ADC 
  	pthread_create(&thread6, NULL, NetworkReceiveThread, NULL);//also be listening for commands to control the digouts.
    pthread_create(&thread5, NULL, NetworkSendThread, NULL); //create pthread that sends message every 1 second
    seven.init();
    seven.setValue(7);
    
  while(1) {
	if(analoginput.getEvent()) {
	  log.push_back(gather_log(&digin1,&digin2,&digin3,&digout1,&digout2,&digout3,&analoginput));
	}
  }
}

void ISR1() { //called when digital input 1 is interrupted
	digin1.update();
	log.push_back(gather_log(&digin1,&digin2,&digin3,&digout1,&digout2,&digout3,&analoginput));
}

void ISR2() {
	digin2.update();
	log.push_back(gather_log(&digin1,&digin2,&digin3,&digout1,&digout2,&digout3,&analoginput));
}

void ISR3() {
	digin3.update();
	log.push_back(gather_log(&digin1,&digin2,&digin3,&digout1,&digout2,&digout3,&analoginput));
}

void* NetworkReceiveThread(void* prt) { //receives commands for digital outputs
	char networkbuffer[MSG_SIZE];
	char* comparestring;
	string buffer;
	while(1) {
		strcpy(networkbuffer, receiveSock.receiveMessage());
		cout << "Command received: " << networkbuffer << endl;
		strncpy(comparestring,networkbuffer,3);
		if(strcmp("led",comparestring) == 0) { //parse command and call corresponding member function
			if(networkbuffer[4] == deviceid+'0') {
				if(networkbuffer[9] == 'n') {
					if(networkbuffer[6] == '1')
						digout1.setValue(1);
					if(networkbuffer[6] == '2')
						digout2.setValue(1);
					if(networkbuffer[6] == '3')
						digout3.setValue(1);
				}
				if(networkbuffer[9] == 'f') {
					if(networkbuffer[6] == '1')
						digout1.setValue(0);
					if(networkbuffer[6] == '2')
						digout2.setValue(0);
					if(networkbuffer[6] == '3')
						digout3.setValue(0);
				}
			}
		}
	}
}

void* NetworkSendThread(void* ptr) { //sends the log every 1 second to the historian
	  int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //intialize timer
		 if(timer_fd == -1) {
				printf("Error creating timer");
				exit(0);
		 }
		 struct itimerspec timerspec; //initialize timer at 1 hz
		 timerspec.it_interval.tv_sec = 1;
		 timerspec.it_interval.tv_nsec = 0; 
		 timerspec.it_value.tv_sec = 0;
		 timerspec.it_value.tv_nsec = 1000000;
		 if(timerfd_settime(timer_fd,0,&timerspec,NULL) == -1) {
			 printf("Error starting timer");
			 exit(0);
		 }
		 struct sched_param param; //set up real time scheduling
		 param.sched_priority = 48;
		 sched_setscheduler(0,SCHED_FIFO,&param);
		 uint64_t num_periods = 0;

		 SocketCommunication sendSock(RSEND_HREC_PORT); //instantiate socket
		 sendSock.sendMessage("#yolo"); //send a dummy initialization message

  	  while(1) {
    		while(read(timer_fd, &num_periods,sizeof(num_periods)) == -1); //wait for timer
    			if(num_periods >  1) {puts("MISSED WINDOW");exit(1);} //error check
    		      log.push_back(gather_log(&digin1,&digin2,&digin3,&digout1,&digout2,&digout3,&analoginput)); //grab update every second
    		      sendSock.sendMessage(log); //send the log
    		      log.erase(log.begin(),log.begin() + log.size()); //erase log after sending
    	  }
  	  //thread should never exit
  	  //pthread_exit((void*)retval);
}

void* ADCthread(void* ptr) { //controls ADC
  int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //intialize timer for 1 khz
	 if(timer_fd == -1) {
		 	printf("Error creating timer");
		 	exit(0);
	 }
	 struct itimerspec timerspec;
	 timerspec.it_interval.tv_sec = 0;
	 timerspec.it_interval.tv_nsec = 1000000;
	 timerspec.it_value.tv_sec = 0;
	 timerspec.it_value.tv_nsec = 1000000;
	 if(timerfd_settime(timer_fd,0,&timerspec,NULL) == -1) {
	 	 printf("Error starting timer");
	 	 exit(0);
	 }
	 struct sched_param param; //schedule as real time task
	 param.sched_priority = 48;
	 sched_setscheduler(0,SCHED_FIFO,&param);
	 uint64_t num_periods = 0;

   AnalogInput *adc = (AnalogInput*)ptr; //grab adc pointer from argument

    while(1) {
      while(read(timer_fd, &num_periods,sizeof(num_periods)) == -1); //wait for timer
  		if(num_periods >  1) {puts("MISSED WINDOW");exit(1);} //error check
           adc->update(); //call update method
    }
    //thread should never exit
    //pthread_exit((void*)retval);
}

struct logEntry gather_log(DigitalInput* digin1, DigitalInput* digin2, DigitalInput* digin3, //gathers statuses,values,notes for the event and packs it into logentry
      DigitalOutput* digout1, DigitalOutput* digout2, DigitalOutput* digout3, AnalogInput* analoginput) { //this is a sad arguments list
  struct logEntry log;
  struct timeval currentTime; //get time stamp and subtract from zero time
  gettimeofday(&currentTime,NULL);
  timersub(&currentTime,&zerotime,&log.timestamp);

  log.deviceid = deviceid; //build everything needed for the logEntry
  log.digin1state = digin1->getValue();
  log.digin2state = digin2->getValue();
  log.digin3state = digin3->getValue();
  log.digout1state = digout1->getValue();
  log.digout2state = digout2->getValue();
  log.digout3state = digout3->getValue();
  log.analoginstate = analoginput->getState();
  log.analogvalue = analoginput->getValue();

  //check all the events and figure out which one actually triggered this, then reset it.
  if(digin1->getEvent()) {
    if(digin1->getValue())
      log.note = "Digital input 1 has gone high";
    else
      log.note = "Digital input 1 has gone low";
    digin1->resetFlag();
    seven.setValue(1);
    }
  else if(digin2->getEvent()) {
    if(digin2->getValue())
      log.note = "Digital input 2 has gone high";
    else
      log.note = "Digital input 2 has gone low";
    digin2->resetFlag();
    seven.setValue(2);
    }
  else if(digin3->getEvent()) {
    if(digin3->getValue())
      log.note = "Digital input 3 has gone high";
    else
      log.note = "Digital input 3 has gone low";
    digin3->resetFlag();
    seven.setValue(3);
    }
  else if(digout1->getEvent()) {
    if(digout1->getValue())
      log.note = "Digital output 1 has gone high";
    else
      log.note = "Digital output 1 has gone low";
    digout1->resetFlag();
    seven.setValue(4);
    }
  else if(digout2->getEvent()) {
    if(digout2->getValue())
      log.note = "Digital output 2 has gone high";
    else
      log.note = "Digital output 2 has gone low";
    digout2->resetFlag();
    seven.setValue(5);
    }
  else if(digout3->getEvent()) {
    if(digout3->getValue())
      log.note = "Digital output 3 has gone high";
    else
      log.note = "Digital output 3 has gone low";
    digout3->resetFlag();
    seven.setValue(6);
    }
  else if(analoginput->getEvent()) {
    int state = analoginput->getState();
    if(state == OVERLOAD) log.note = "Analog input has detected a line overload";
    if(state == UNDERLOAD) log.note = "Analog input has detected a line underload";
    if(state == POWERDOWN) log.note = "Analog input has detected loss of power";
    analoginput->resetFlag();
    seven.setValue(7);
  }
  else log.note = "No event has occured. This is just a 1 Hz update";
  return log;
}
