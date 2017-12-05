/*

RTU program

*/
#include "finalProject.h"

void* ADCthread(void*);

//max made a change to test github
//max made another change to test github


//pthread
//needs to run at 1khz
//read adc

int main() {
  //instantiate all objects
    AnalogInput analoginput(); //analog input
    pthread_t thread1; //create thread pointer
    pthread_create(&thread1, NULL, ADCthread, (void*)&analoginput); //execute thread that will time ADC
    //digital inputs
    //digital outputs
    //networking stuff
    
    
  while(1) {  
  //monitor flags from inputs
   //if (flag)
     // //grab states and write to a log file
  }

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
    adc->updateADC(); //call update method
  }
  
  //thread should never exit
  //pthread_exit((void*)retval);
}