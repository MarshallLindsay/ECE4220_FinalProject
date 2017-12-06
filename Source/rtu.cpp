/*

RTU program

*/
#include "finalProject.h"

void* ADCthread(void*);
void gpio_tests();
void check_status();

int main() {
  //instantiate all objects
    wiringPiSetup();
    AnalogInput analoginput(); //analog input
    pthread_t thread1; //create thread pointer
    pthread_create(&thread1, NULL, ADCthread, (void*)&analoginput); //execute thread that will time ADC
    DigitalInput digin1(1);
    DigitalInput digin2(2);
    DigitalInput digin3(3);
    DigitalOutput digout1(4);
    DigitalOutput digout2(5);
    DigitalOutput digout3(6);
    vector<struct logEntry> log; //make this a vector eventually 
  while(1) {  
    digin1.update();
    digin2.update();
    digin3.update();
    log.push_back(check_status());
  
   //if (flag)
     // //grab states and write to a log file
  }

}

void gpio_tests() {

}

void check_status() {

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