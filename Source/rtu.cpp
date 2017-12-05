/*

RTU program

*/


AnalogInput::AnalogInput() {
  if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
  		printf("wiringPiSPISetup failed\n");
  		exit(0);
  	}
   pthread_t thread1; //create thread pointer
   pthread_create(&thread1, NULL, (void *)&ADCthread, NULL); //execute thread
   //set up the timer to call get_ADC at 1000 Hz
   
   //
}
  
uint16_t AnalogInput::get_ADC() {
  uint8_t spiData[3];
	spiData[0] = 0b00000001; // Contains the Start Bit
	spiData[1] = 0b10000000 | (ADC_CHANNEL << 4);	// Mode and Channel: M0XX0000
												// M = 1 ==> single ended
									// XX: channel selection: 00, 01, 10 or 11
	spiData[2] = 0;	// "Don't care", doesn't matter the value.
	
	// The next function performs a simultaneous write/read transaction over the selected
	// SPI bus. Data that was in the spiData buffer is overwritten by data returned from
	// the SPI bus.
	wiringPiSPIDataRW(SPI_CHANNEL, spiData, 3);
	
	// spiData[1] and spiData[2] now have the result (2 bits and 8 bits, respectively)
	
	this.value =  (3.3/1024)*((spiData[1] << 8) | spiData[2]);
}

void AnalogInput::test_ADC() {
  cout << "ADC value is " << get_ADC() << "\n";
}

void AnalogInput::ADCthread() {
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
   
  while(1) {
    while(read(timer_fd, &num_periods,sizeof(num_periods)) == -1); //wait for timer
		if(num_periods >  1) {puts("MISSED WINDOW");exit(1);} //error check
    get_ADC();
    if(this.value < ADC_OVERLOAD
  }
  
  //thread should never exit
  //pthread_exit((void*)retval);
}

//max made a change to test github
//max made another change to test github


//pthread
//needs to run at 1khz
//read adc

int main {
  //instantiate all objects
    //analog inputs
    //digital inputs
    //digital outputs
    //networking stuff
    
    
  while(1) {  
  //monitor flags from inputs
   //if (flag)
     // //grab states and write to a log file
      
  }

}