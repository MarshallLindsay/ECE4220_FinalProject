/*

RTU program

*/
#include "finalProject.h"

void* ADCthread(void*);

void gpio_tests();
int deviceid;
struct logEntry gather_log(DigitalInput digin1, DigitalInput digin2, DigitalInput digin3, 
      DigitalOutput digout1, DigitalOutput digout2, DigitalOutput digout3, AnalogInput analoginput);

int main(int argc, char **argv) {
    if(argc != 2) { //check inputs and set device id
      cout << "Usage: rtu <deviceid>\r\n";
      exit(0);
    }
    deviceid = (int) argv[1]; 

  //instantiate all objects
    wiringPiSetup();
    AnalogInput analoginput; //analog input
    pthread_t thread1; //create thread pointer
    pthread_create(&thread1, NULL, ADCthread, (void*)&analoginput); //execute thread that will time ADC
    DigitalInput digin1(26);
    DigitalInput digin2(23);
    DigitalInput digin3(28);
    DigitalOutput digout1(9);
    DigitalOutput digout2(7);
    DigitalOutput digout3(21);
    vector<struct logEntry> log; //make this a vector eventually 
    
    //instantiate networking stuff
      //create pthread that sends data every 1 second
      //also be listening for commands to control the digouts. call get_states() and make a log entry when you receive a command.
  while(1) {  
  //  network.update();  //read network buffer for incoming commands
	 cout<<digin1.getEvent()<<endl;
    digin1.update();
    digin2.update();
    digin3.update();
/*    if(digin1.getEvent()) cout << "event on digin1" << endl;
    if(digin2.getEvent()) cout << "event on digin2" << endl;
    if(digin3.getEvent()) cout << "event on digin3" << endl;
    if(digout1.getEvent()) cout << "event on digout1" << endl;
    if(digout2.getEvent()) cout << "event on digout2" << endl;
    if(digout3.getEvent()) cout << "event on digout3" << endl;
    if(analoginput.getEvent()) cout << "event on analog" << endl;*/
  //  cout << "state of analog is " << analoginput.getState() << endl;}
    cout<<digin1.getEvent()<<endl;
    if(digin1.getEvent() || digin2.getEvent() || digin3.getEvent() || analoginput.getEvent()) {
      log.push_back(gather_log(digin1,digin2,digin3,digout1,digout2,digout3,analoginput));
    	cout << log[log.size()-1].note << endl; 
    //	cout << "event detected" << endl;
  }  
    cout<<digin1.getEvent()<<endl;
  }
}

void gpio_tests() {

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
	 param.sched_priority = 49;
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

struct logEntry gather_log(DigitalInput digin1, DigitalInput digin2, DigitalInput digin3, 
      DigitalOutput digout1, DigitalOutput digout2, DigitalOutput digout3, AnalogInput analoginput) { //this is a sad arguments list
  struct logEntry log;
  gettimeofday(&log.timestamp,NULL);
  log.deviceid = deviceid;
  log.digin1state = digin1.getValue();
  log.digin2state = digin2.getValue();
  log.digin3state = digin3.getValue();
  log.digout1state = digout1.getValue();
  log.digout2state = digout2.getValue();
  log.digout3state = digout3.getValue();
  log.analoginstate = analoginput.getState();
  log.analogvalue = analoginput.getValue();
  
  //check all the events
  if(digin1.getEvent()) {
    if(digin1.getValue())
      log.note = "Digital input 1 has gone high";
    else
      log.note = "Digital input 1 has gone low";
    digin1.resetFlag();
    }
  else if(digin2.getEvent()) {
    if(digin2.getValue())
      log.note = "Digital input 2 has gone high";
    else
      log.note = "Digital input 2 has gone low";
    digin2.resetFlag();
    }
  else if(digin3.getEvent()) {
    if(digin3.getValue())
      log.note = "Digital input 3 has gone high";
    else
      log.note = "Digital input 3 has gone low";
    digin3.resetFlag();
    }
  else if(digout1.getEvent()) {
    if(digout1.getValue())
      log.note = "Digital output 1 has gone high";
    else
      log.note = "Digital output 1 has gone low";
    digout1.resetFlag();
    }
  else if(digout2.getEvent()) {
    if(digout2.getValue())
      log.note = "Digital output 2 has gone high";
    else
      log.note = "Digital output 2 has gone low";
    digout2.resetFlag();
    }
  else if(digout3.getEvent()) {
    if(digout3.getValue())
      log.note = "Digital output 3 has gone high";
    else
      log.note = "Digital output 3 has gone low";
    digout3.resetFlag();
    }
  else if(analoginput.getEvent()) {
    int state = analoginput.getState();
    if(state == OVERLOAD) log.note = "Analog input has detected a line overload";
    if(state == UNDERLOAD) log.note = "Analog input has detected a line underload";
    if(state == POWERDOWN) log.note = "Analog input has detected loss of power";
    analoginput.resetFlag();
  }
  else log.note = "No event has occured. This is just a 1 Hz update";  
    
  return log;
}