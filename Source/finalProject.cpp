/*
Marshall Lindsay
Max Houck
ECE 4220 Final Final Project
*/

#include "finalProject.h"


//Socket communication methods
SocketCommunication::SocketCommunication(){
  //Create a socket, connectionless
  this->sockfd= socket(AF_INET, SOCK_DGRAM, 0);
	if(this->sockfd < 0){
		cout<<"\nSocket creation failed"<<endl;
		exit(1);
	}

  //Set the port.. We could change to dynamic port
  this->portno = 2345;
  //cout<<"hello"<<this->portno<<endl;

  //Set the boolval.. Just gotta do it
  this->boolval = 1;

  //Clear all of the server data
  this->length = sizeof(struct sockaddr_in);
  bzero((char*)&(this->serveraddress), sizeof(this->serveraddress));

  //Set the address family as IPv4
  this->serveraddress.sin_family = AF_INET;

  //Set the address to INADDR_ANY
  this->serveraddress.sin_addr.s_addr = INADDR_ANY;

  //Set the port number
  this->serveraddress.sin_port = htons(this->portno);

  //Bind the socket
  if(bind(this->sockfd, (struct sockaddr *)&(this->serveraddress), sizeof(this->serveraddress)) < 0){
		cout<<"\nFailed to bind the socket!"<<endl;
		exit(1);
	}

  //Set the option for broadcasting
  if (setsockopt(this->sockfd, SOL_SOCKET, SO_BROADCAST, &(this->boolval), sizeof(this->boolval)) < 0)
	{
		cout<<"\nError setting socket options"<<endl;
		exit(1);
	}
  //Get the ip of the system
  //Pulled from stackoverflow. Modified slightly for my uses
	//https://stackoverflow.com/questions/579783/how-to-detect-ip-address-change-programmatically-in-linux
  struct ifreq ifr;
  strncpy(ifr.ifr_name, "wlan0", sizeof(ifr.ifr_name));
  if(ioctl(this->sockfd, SIOCGIFADDR, &ifr) >= 0){
    this->localAddress = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    //cout<<"\nMy IP addr is: "<<this->localAddress<<endl;
  }

  this->fromlen = sizeof(struct sockaddr_in);

}

//Overloaded constructor to allow for dynamic port declaration
SocketCommunication::SocketCommunication(int port){
  //Create a socket, connectionless
  this->sockfd= socket(AF_INET, SOCK_DGRAM, 0);
	if(this->sockfd < 0){
		cout<<"\nSocket creation failed"<<endl;
		exit(1);
	}

  //Set the port.. We could change to dymanic port
  this->portno = port;
  //cout<<"hello"<<this->portno<<endl;

  //Set the boolval.. Just gotta do it
  this->boolval = 1;

  //Clear all of the server data
  this->length = sizeof(struct sockaddr_in);
  bzero((char*)&(this->serveraddress), sizeof(this->serveraddress));

  //Set the address family as IPv4
  this->serveraddress.sin_family = AF_INET;

  //Set the address to INADDR_ANY
  this->serveraddress.sin_addr.s_addr = INADDR_ANY;

  //Set the port number
  this->serveraddress.sin_port = htons(this->portno);

  //Bind the socket
  if(bind(this->sockfd, (struct sockaddr *)&(this->serveraddress), sizeof(this->serveraddress)) < 0){
		cout<<"\nFailed to bind the socket!"<<endl;
		exit(1);
	}

  //Set the option for broadcasting
  if (setsockopt(this->sockfd, SOL_SOCKET, SO_BROADCAST, &(this->boolval), sizeof(this->boolval)) < 0)
	{
		cout<<"\nError setting socket options"<<endl;
		exit(1);
	}
  //Get the ip of the system
  //Pulled from stackoverflow. Modified slightly for my uses
	//https://stackoverflow.com/questions/579783/how-to-detect-ip-address-change-programmatically-in-linux
  struct ifreq ifr;
  strncpy(ifr.ifr_name, "wlan0", sizeof(ifr.ifr_name));
  if(ioctl(this->sockfd, SIOCGIFADDR, &ifr) >= 0){
    this->localAddress = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    //cout<<"\nMy IP addr is: "<<this->localAddress<<endl;
  }

  this->fromlen = sizeof(struct sockaddr_in);

}

SocketCommunication::~SocketCommunication(){
  //Close the socket
  close(this->sockfd);
}

int SocketCommunication::sendMessage(string buffer){
  //Set the broadcast IP
  this->serveraddress.sin_addr.s_addr = inet_addr("128.206.19.255");
  //Convert the string to a char*
  const char * message = buffer.c_str();
  int n;
  //Send the message
  n = sendto(this->sockfd, message, MSG_SIZE, 0, (struct sockaddr*)&(this->serveraddress), this->fromlen);
  //Error check
  if(n < 0){
    cout<<"SEND FAILED"<<endl;
    exit(1);
  }
  return(1);
}

//Overload of send message to send a log.
int SocketCommunication::sendMessage(struct logEntry buffer){
  //Set the broadcast IP
  this->serveraddress.sin_addr.s_addr = inet_addr("128.206.19.255");
  int n;
  //Convert the different members of the struct to CSV string
  string temp;
  temp += to_string(buffer.analoginstate);
  temp += ",";
  temp += to_string(buffer.digin1state);
  temp += ",";
  temp += to_string(buffer.digin2state);
  temp += ",";
  temp += to_string(buffer.digin3state);
  temp += ",";
  temp += to_string(buffer.digout1state);
  temp += ",";
  temp += to_string(buffer.digout2state);
  temp += ",";
  temp += to_string(buffer.digout3state);
  temp += ",";
  temp += to_string(buffer.analogvalue);
  temp += ",";
  temp += to_string(buffer.timestamp.tv_sec);
  temp += ",";
  temp += to_string(buffer.timestamp.tv_usec);
  temp += ",";
  temp += to_string(buffer.deviceid);
  temp += ",";
  temp += buffer.note;

  //Convert the string to a char*
  const char * message = temp.c_str();
  //Send the message
  n = sendto(this->sockfd, message, MSG_SIZE, 0, (struct sockaddr*)&(this->serveraddress), this->fromlen);
  //Error check
  if(n < 0){
    cout<<"SEND FAILED"<<endl;
    exit(1);
  }
}

//Overload of sendMessage to send a vector of logs.
//Does what sendMessage(struct logEvent) with the addition of a for loop to
//go through the whole vector.
int SocketCommunication::sendMessage(vector<struct logEntry> buffer){
	for(int i = 0; i < buffer.size(); i++) {
	  this->serveraddress.sin_addr.s_addr = inet_addr("128.206.19.255");
	  int n;
	  string temp;
	  temp += to_string(buffer[i].analoginstate);
	  temp += ",";
	  temp += to_string(buffer[i].digin1state);
	  temp += ",";
	  temp += to_string(buffer[i].digin2state);
	  temp += ",";
	  temp += to_string(buffer[i].digin3state);
	  temp += ",";
	  temp += to_string(buffer[i].digout1state);
	  temp += ",";
	  temp += to_string(buffer[i].digout2state);
	  temp += ",";
	  temp += to_string(buffer[i].digout3state);
	  temp += ",";
	  temp += to_string(buffer[i].analogvalue);
	  temp += ",";
	  temp += to_string(buffer[i].timestamp.tv_sec);
	  temp += ",";
	  temp += to_string(buffer[i].timestamp.tv_usec);
	  temp += ",";
	  temp += to_string(buffer[i].deviceid);
	  temp += ",";
	  temp += buffer[i].note;

	  const char * message = temp.c_str();
	  cout<<message<<endl;
	  n = sendto(this->sockfd, message, MSG_SIZE, 0, (struct sockaddr*)&(this->serveraddress), this->fromlen);

	  if(n < 0){
		cout<<"SEND FAILED"<<endl;
		exit(1);
	  }
	}

  return(1);
}

char* SocketCommunication::receiveMessage(void){
  //Clear the recMessage field
  bzero(this->recMessage, MSG_SIZE);
  int n;
  //Get the message and copy to this->recMessage
  n = recvfrom(this->sockfd, this->recMessage, MSG_SIZE, 0,(struct sockaddr*)&(this->fromaddress), &(this->fromlen));
  //Return the message
  return this->recMessage;
}

//The following methods are only used in the RTU and wont be compiled on
//the workstation.
#ifdef RTU
AnalogInput::AnalogInput() {
	this->eventFlag = false;
    if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
  		printf("wiringPiSPISetup failed\n");
  		exit(0);
  	}
   this->state = OK;
}


void AnalogInput::get_ADC() { //basically the same as Luis' example function, except sets the value
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

	this->value =  (3.3/1024)*((spiData[1] << 8) | spiData[2]); //assuming reference will always be a stable 3.3
}

void AnalogInput::test_ADC() { //used for debugging only
  cout << "ADC value is " << this->value << "\n";
}

int AnalogInput::getState() { //returns state of adc - is it normal, or which error condition are we in?
  return this->state;
}

void AnalogInput::resetFlag() { //resets the flag and changes state according to the state diagram shown in report
  this->eventFlag = false;
  if(this->state == OVERLOAD) {this->state = ACK1; this->count = 0;}
  if(this->state == UNDERLOAD){this->state = ACK2; }
  if(this->state == POWERDOWN)this->state = ACK3;
}

bool AnalogInput::getEvent() {
  return this->eventFlag;
}

double AnalogInput::getValue() {
  return this->value;
}

AnalogInput::~AnalogInput() {
}

void AnalogInput::update() { //see state diagram for a better understanding of how this works
   get_ADC(); //actually get the value over spi

   if(this->state == OK) { //depending on state, check for events on ADC
	  if(this->value <= (this->last + ADC_TOLERANCE) && this->value >= (this->last - ADC_TOLERANCE)) //power down status can happen even if overload is already set
		this->count++;
	  else {
		this->count = 0;
		this->last = this->value;
	  }
	  if(this->value > ADC_OVERLOAD) { //check the value and update status if necessary
		this->state = OVERLOAD;
		this->eventFlag = true;
	  }
	  if(this->value < ADC_UNDERLOAD) {
		this->state = UNDERLOAD;
		this->eventFlag = true;
	  }
	  if(this->count > ADC_POWERDOWN) {
		this->state = POWERDOWN;
		this->eventFlag = true;
	  }
   }

   if(this->state == ACK1) {
	   if(this->value < ADC_OVERLOAD)
		   state = OK;
   	   }

   if(this->state == ACK2) {
	   if(this->value <= (this->last + ADC_TOLERANCE) && this->value >= (this->last - ADC_TOLERANCE)) //power down status can happen even if overload is already set
	  		this->count++;
	   else {
		   this->count = 0;
		   this->last = this->value;
	   }
	   if(this->value > ADC_UNDERLOAD) {
		   	this->state = OK;
	   }
	   if(this->count > ADC_POWERDOWN) {
			this->state = POWERDOWN;
			this->eventFlag = true;
	   }
   }

   if(this->state == ACK3) {
	   if(this->value >= (this->last + ADC_TOLERANCE) || this->value <= (this->last - ADC_TOLERANCE)) {
		   this->state = OK;
		   this->count = 0;
		   this->last = this->value;
	   }
   }
}

DigitalInput::DigitalInput(int pin) {
	this->pinNumber = pin;
	this->eventFlag = false;
	pinMode(pin,INPUT);
	pullUpDnControl(pin, PUD_DOWN);
	usleep(1000); //tried this to get this to work if the switch starts as a 1 but doesn't seem to do the trick
	this->value = digitalRead(this->pinNumber);
}

void DigitalInput::update() {
  if(this->value != digitalRead(this->pinNumber)) {   //use wiringpi to check if value on pin is different than value in object
    this->eventFlag = true; //if theyre different set the event flag
  }
 this->value = digitalRead(this->pinNumber); //always update the value
}
bool DigitalInput::getEvent() {
  return this->eventFlag;
}

void DigitalInput::resetFlag() {
  this->eventFlag = false;
}

int DigitalInput::getValue() {
  return this->value;
}

DigitalOutput::DigitalOutput(int pin,int outputNumber) {
	this->outputNumber = outputNumber; //output number is either 1, 2, or 3
	this->eventFlag = false;
  this->pinNumber = pin; //pin number is the wiring pi pin
  this->value = digitalRead(this->pinNumber);
  if((cdev_id = open(CHAR_DEV, O_WRONLY)) == -1) {   // Open the Character Device for writing
      printf("Cannot open device %s\n", CHAR_DEV);
      exit(1);
  }
}

void DigitalOutput::setValue(int value) {
  this->eventFlag = true; //uses character device and kernel module to set the outputs
  char buffer[2]; //parse message for char device and send it
  buffer[0] = this->outputNumber + '0';
  buffer[1] = value + '0';
  int dummy = write(cdev_id, buffer, sizeof(buffer));
	if(dummy != sizeof(buffer)) { //error check
	  printf("Write failed, leaving...\n");
	  exit(0);
	}
}

bool DigitalOutput::getEvent() {
  return this->eventFlag;
}

void DigitalOutput::resetFlag() {
  this->eventFlag = false;
}

int DigitalOutput::getValue() {
  return this->value;
}

_7seg::_7seg() {
}

_7seg::~_7seg() {

}

void _7seg::init() {
	pinMode(SEVENENABLE,OUTPUT);
	pinMode(SEVENA,OUTPUT);
	pinMode(SEVENB,OUTPUT);
	pinMode(SEVENC,OUTPUT);
	pinMode(SEVEND,OUTPUT);
	digitalWrite(SEVENENABLE,1);
	this->setValue(0);

}
void _7seg::setValue(int num) {
	switch(num) {
	case 0:
		digitalWrite(SEVENA,0);
		digitalWrite(SEVENB,0);
		digitalWrite(SEVENC,0);
		digitalWrite(SEVEND,0);
		break;
	case 1:
		digitalWrite(SEVENA,1);
		digitalWrite(SEVENB,0);
		digitalWrite(SEVENC,0);
		digitalWrite(SEVEND,0);
		break;
	case 2:
		digitalWrite(SEVENA,0);
		digitalWrite(SEVENB,1);
		digitalWrite(SEVENC,0);
		digitalWrite(SEVEND,0);
		break;
	case 3:
		digitalWrite(SEVENA,1);
		digitalWrite(SEVENB,1);
		digitalWrite(SEVENC,0);
		digitalWrite(SEVEND,0);
		break;
	case 4:
		digitalWrite(SEVENA,0);
		digitalWrite(SEVENB,0);
		digitalWrite(SEVENC,1);
		digitalWrite(SEVEND,0);
		break;
	case 5:
		digitalWrite(SEVENA,1);
		digitalWrite(SEVENB,0);
		digitalWrite(SEVENC,1);
		digitalWrite(SEVEND,0);
		break;
	case 6:
		digitalWrite(SEVENA,0);
		digitalWrite(SEVENB,1);
		digitalWrite(SEVENC,1);
		digitalWrite(SEVEND,0);
		break;
	case 7:
		digitalWrite(SEVENA,1);
		digitalWrite(SEVENB,1);
		digitalWrite(SEVENC,1);
		digitalWrite(SEVEND,0);
		break;
	default: break;
	}
}
#endif
