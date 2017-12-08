/*

RTU program

*/
#include "finalProject.h"

void* ADCthread(void*);
void* DiginThread(void* ptr);
void* NetworkSendThread(void* ptr);
void* NetworkReceiveThread(void* prt);
void ISR1();
void ISR2();
void ISR3();
DigitalInput digin1(26);
DigitalInput digin2(23);
DigitalInput digin3(28);
DigitalOutput digout1(9,1);
DigitalOutput digout2(7,2);
DigitalOutput digout3(21,3);
vector<struct logEntry> log;
struct timeval zerotime;
AnalogInput analoginput; 

SocketCommunication receiveSock(HSEND_RREC_PORT);
int deviceid;
struct logEntry gather_log(DigitalInput* digin1, DigitalInput* digin2, DigitalInput* digin3,
      DigitalOutput* digout1, DigitalOutput* digout2, DigitalOutput* digout3, AnalogInput* analoginput);

int main(int argc, char **argv) {
    if(argc != 2 || *argv[1] > '9' || *argv[1] < '0') { //check inputs and set device id
      cout << "Usage: rtu <deviceid>\r\nDevice ID must be 1 digit";
      exit(0);
    }
    deviceid = *argv[1] - '0'; //this is sneaky
    
    receiveSock.receiveMessage(); //set the zero time    
    gettimeofday(&zerotime,NULL);
  //instantiate all objects
    wiringPiSetup();
  //  AnalogInput analoginput; //analog input
 //   DigitalInput digin1(26);
 //   DigitalInput digin2(23);
 //   DigitalInput digin3(28);
 //   DigitalOutput digout1(9);
//   DigitalOutput digout2(7);
//    DigitalOutput digout3(21);
    pthread_t thread1,thread2,thread3,thread4,thread5,thread6; //create thread pointer
	pthread_create(&thread1, NULL, ADCthread, (void*)&analoginput); //execute thread that will time ADCpthread_t thread1,thread2,thread3,thread4; //create thread pointer
//	pthread_create(&thread2, NULL, DiginThread, (void*)&digin1); //execute thread that will time ADCpthread_t thread1,thread2,thread3,thread4; //create thread pointer
//	pthread_create(&thread3, NULL, DiginThread, (void*)&digin2); //execute thread that will time ADCpthread_t thread1,thread2,thread3,thread4; //create thread pointer
//	pthread_create(&thread4, NULL, DiginThread, (void*)&digin3); //execute thread that will time ADC
    //make this a vector eventually
	pthread_create(&thread6, NULL, NetworkReceiveThread, NULL);//also be listening for commands to control the digouts.
    pthread_create(&thread5, NULL, NetworkSendThread, NULL); //create pthread that sends data every 1 second
    
    int dummy1 = wiringPiISR(26,INT_EDGE_BOTH,&ISR1); //initialize interrupts for the inputs
    int dummy2 = wiringPiISR(23,INT_EDGE_BOTH,&ISR2);
    int dummy3 = wiringPiISR(28,INT_EDGE_BOTH,&ISR3);
  while(1) {
    if(analoginput.getEvent()) {
      log.push_back(gather_log(&digin1,&digin2,&digin3,&digout1,&digout2,&digout3,&analoginput));
    //	cout << log[log.size()-1].note << endl;
    }
  }
}

void ISR1() {
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

void* NetworkReceiveThread(void* prt) {
	char networkbuffer[MSG_SIZE];
	char* comparestring;
	string buffer;
	
	while(1) {
		strcpy(networkbuffer, receiveSock.receiveMessage());
		strncpy(comparestring,networkbuffer,3);
		if(strcmp("led",comparestring) == 0) {
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

void* NetworkSendThread(void* ptr) {
		
	  //timer and scheduling setup
	  int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //intialize realtime scheduling
		 if(timer_fd == -1) {
				printf("Error creating timer");
				exit(0);
		 }
		 struct itimerspec timerspec;
		 timerspec.it_interval.tv_sec = 1;
		 timerspec.it_interval.tv_nsec = 0; //1 hz
		 timerspec.it_value.tv_sec = 0;
		 timerspec.it_value.tv_nsec = 1000000;
		 if(timerfd_settime(timer_fd,0,&timerspec,NULL) == -1) {
			 printf("Error starting timer");
			 exit(0);
		 }
		 struct sched_param param;
		 param.sched_priority = 48;
		 sched_setscheduler(0,SCHED_FIFO,&param);
		 uint64_t num_periods = 0;
	
		 SocketCommunication sendSock(RSEND_HREC_PORT);
	
		 sendSock.sendMessage("#yolo");
		 
	  while(1) {
		while(read(timer_fd, &num_periods,sizeof(num_periods)) == -1); //wait for timer
			if(num_periods >  1) {puts("MISSED WINDOW");exit(1);} //error check
			
		      log.push_back(gather_log(&digin1,&digin2,&digin3,&digout1,&digout2,&digout3,&analoginput));
		 //     cout << log[log.size()-1].note << endl;
		      sendSock.sendMessage(log);
		      log.erase(log.begin(),log.begin() + log.size());
	  }
	
	  //thread should never exit
	  //pthread_exit((void*)retval);
}

void* DiginThread(void* ptr) {

	 struct sched_param param;
	 param.sched_priority = 49;
	 sched_setscheduler(0,SCHED_FIFO,&param);

	 DigitalInput *digin = (DigitalInput*)ptr;

  while(1) {
         digin->update(); //call update method
  }

  //thread should never exit
  //pthread_exit((void*)retval);
}

void* ADCthread(void* ptr) {
  //timer and scheduling setup
  int timer_fd = timerfd_create(CLOCK_MONOTONIC, 0); //intialize realtime scheduling
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
	 struct sched_param param;
	 param.sched_priority = 48;
	 sched_setscheduler(0,SCHED_FIFO,&param);
	 uint64_t num_periods = 0;

   AnalogInput *adc = (AnalogInput*)ptr;

  while(1) {
    while(read(timer_fd, &num_periods,sizeof(num_periods)) == -1); //wait for timer
		if(num_periods >  1) {puts("MISSED WINDOW");exit(1);} //error check
         adc->update(); //call update method
  }

  //thread should never exit
  //pthread_exit((void*)retval);
}
//TODO: marshall can just copy that ADCthread and make the timer 1 second and have it call his networking send() function

struct logEntry gather_log(DigitalInput* digin1, DigitalInput* digin2, DigitalInput* digin3,
      DigitalOutput* digout1, DigitalOutput* digout2, DigitalOutput* digout3, AnalogInput* analoginput) { //this is a sad arguments list
  struct logEntry log;
  struct timeval currentTime;
  gettimeofday(&currentTime,NULL);
  timersub(&currentTime,&zerotime,&log.timestamp);
  
  log.deviceid = deviceid;
  log.digin1state = digin1->getValue();
  log.digin2state = digin2->getValue();
  log.digin3state = digin3->getValue();
  log.digout1state = digout1->getValue();
  log.digout2state = digout2->getValue();
  log.digout3state = digout3->getValue();
  log.analoginstate = analoginput->getState();
  log.analogvalue = analoginput->getValue();

  //check all the events
  if(digin1->getEvent()) {
    if(digin1->getValue())
      log.note = "Digital input 1 has gone high";
    else
      log.note = "Digital input 1 has gone low";
    digin1->resetFlag();
    }
  else if(digin2->getEvent()) {
    if(digin2->getValue())
      log.note = "Digital input 2 has gone high";
    else
      log.note = "Digital input 2 has gone low";
    digin2->resetFlag();
    }
  else if(digin3->getEvent()) {
    if(digin3->getValue())
      log.note = "Digital input 3 has gone high";
    else
      log.note = "Digital input 3 has gone low";
    digin3->resetFlag();
    }
  else if(digout1->getEvent()) {
    if(digout1->getValue())
      log.note = "Digital output 1 has gone high";
    else
      log.note = "Digital output 1 has gone low";
    digout1->resetFlag();
    }
  else if(digout2->getEvent()) {
    if(digout2->getValue())
      log.note = "Digital output 2 has gone high";
    else
      log.note = "Digital output 2 has gone low";
    digout2->resetFlag();
    }
  else if(digout3->getEvent()) {
    if(digout3->getValue())
      log.note = "Digital output 3 has gone high";
    else
      log.note = "Digital output 3 has gone low";
    digout3->resetFlag();
    }
  else if(analoginput->getEvent()) {
    int state = analoginput->getState();
    if(state == OVERLOAD) log.note = "Analog input has detected a line overload";
    if(state == UNDERLOAD) log.note = "Analog input has detected a line underload";
    if(state == POWERDOWN) log.note = "Analog input has detected loss of power";
    analoginput->resetFlag();
  }
  else log.note = "No event has occured. This is just a 1 Hz update";

  return log;
}
