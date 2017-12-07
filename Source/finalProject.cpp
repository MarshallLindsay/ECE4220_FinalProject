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

  //Set the port.. We could change to dymanic port
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
    cout<<"\nMy IP addr is: "<<this->localAddress<<endl;
  }

  //Set the fromaddress struct as the same as the server.
  this->fromaddress = this->serveraddress;
  //Set the address to the broadcast address for the lab
  this->fromaddress.sin_addr.s_addr = inet_addr("128.206.19.255");
  //Set the size for the fromlen
  this->fromlen = sizeof(struct sockaddr_in);

}

SocketCommunication::~SocketCommunication(){
  //Close the socket
  close(this->sockfd);
}

int SocketCommunication::sendMessage(string buffer){
  this->fromaddress.sin_addr.s_addr = inet_addr("128.206.19.255");
  const char * message = buffer.c_str();
  int n;
  cout<<message<<endl;
  n = sendto(this->sockfd, message, MSG_SIZE, 0, (struct sockaddr*)&(this->fromaddress), this->fromlen);

  if(n < 0){
    cout<<"SEND FAILED"<<endl;
    exit(1);
  }
  return(1);
}

int SocketCommunication::sendMessage(struct logEntry buffer){
  this->fromaddress.sin_addr.s_addr = inet_addr("128.206.19.255");
  int n;
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

  const char * message = temp.c_str();
  cout<<message<<endl;
  n = sendto(this->sockfd, message, MSG_SIZE, 0, (struct sockaddr*)&(this->fromaddress), this->fromlen);

  if(n < 0){
    cout<<"SEND FAILED"<<endl;
    exit(1);
  }
  return(1);
}

char* SocketCommunication::receiveMessage(void){
  char buffer[MSG_SIZE];
  bzero(&(this->receive), MSG_SIZE);
  int n;
  n = recvfrom(this->sockfd, buffer, MSG_SIZE, 0,(struct sockaddr*)&(this->fromaddress), &(this->fromlen));
  bzero(buffer, MSG_SIZE);
  cout<<buffer<<endl;
  return 0;

}

AnalogInput::AnalogInput() {
	this->eventFlag = false;
  if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) < 0) {
  		printf("wiringPiSPISetup failed\n");
  		exit(0);
  	}
   this->state = OK;
}

void AnalogInput::get_ADC() {
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

	this->value =  (3.3/1024)*((spiData[1] << 8) | spiData[2]);
}

void AnalogInput::test_ADC() {
  cout << "ADC value is " << this->value << "\n";
}

int AnalogInput::getState() {
  return this->state;
}

void AnalogInput::resetFlag() {
  this->eventFlag = false;
  this->state = OK;
}

bool AnalogInput::getEvent() {
  return this->eventFlag;
}

double AnalogInput::getValue() {
  return this->value;
}

AnalogInput::~AnalogInput() {
}

void AnalogInput::update() {
   get_ADC(); //actually get the value over spi

    if(this->value == this->last) //power down status can happen even if overload is already set
      this->count++;
    if(this->value != this->last)
      this->count = 0;
    if(this->count > ADC_POWERDOWN)
      this->state = POWERDOWN;

    if (this->state == OK) { //overload or underload states cannot occur when power is already down
      if(this->value > ADC_OVERLOAD) //check the value and update status if necessary
        this->state = OVERLOAD;
      if(this->value < ADC_UNDERLOAD)
        this->state = UNDERLOAD;
    }
    this->last = this->value;
    if(this->state != OK) {
    	this->eventFlag = true;
    }

    //cout << this->value << endl; //for debugging
}

DigitalInput::DigitalInput(int pin) {
 //set pull up or pull down if necessary
 //set pin as input
	this->pinNumber = pin;
	this->eventFlag = false;
	pinMode(pin,INPUT);
	pullUpDnControl(pin, PUD_DOWN);
	this->value = digitalRead(this->pinNumber);
}

void DigitalInput::update() {
  //use wiringpi to check if value on pin is different than value in object
	if(this->pinNumber == 26)
	cout << "at start of update for pin previous value is " << this->value << " new value is " << digitalRead(this->pinNumber) << "event flag is " << this->eventFlag <<  " the value of comparison is " << (this->value != digitalRead(this->pinNumber)) <<endl;
  if(this->value != digitalRead(this->pinNumber)) {
	  cout <<"setting event flag to true" <<endl;
    this->eventFlag = true; //if theyre different set the event flag
     //update the value anyways
  }
 this->value = digitalRead(this->pinNumber);
 if(this->pinNumber == 26)
  cout << "at end of update current value is " << digitalRead(this->pinNumber) << "and eventflag is " << this->eventFlag << endl;

}
bool DigitalInput::getEvent() {
  return this->eventFlag;
}

void DigitalInput::resetFlag() {
  this->eventFlag = false;
  cout << "resetting flag. flag is " << this->eventFlag << endl;
}

int DigitalInput::getValue() {
  return this->value;
}

DigitalOutput::DigitalOutput(int pin) {
	this->eventFlag = false;
  this->pinNumber = pin;
  pinMode(this->pinNumber,OUTPUT);
  this->value = digitalRead(this->pinNumber);
}

void DigitalOutput::setValue(int value) {
  digitalWrite(this->pinNumber,value);
  this->eventFlag = true;
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
