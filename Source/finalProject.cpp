/*
Marshall Lindsay
Max Houck
ECE 4220 Final Final Project
*/

#include "finalProject.h"


//Socket communication methods
SocketCommunication::SocketCommunication(){
  int sockfd1, newsockfd, portno1, pid, j = 0;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  int boolval1;

  //Create a socket, connectionless
  sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		cout<<"\nSocket creation failed"<<endl;
		exit(1);
	}

  //Set the port.. We could change to dymanic port
  portno1 = 2345;
  //cout<<"hello"<<this->portno<<endl;
  this->portno = portno1;

  //Set the boolval.. Just gotta do it
  this->boolval = 1;

  //Clear all of the server data
  this->length = sizeof(struct sockaddr_in);
  bzero((char*)&serv_addr, sizeof(serv_addr));

  //Set the address family as IPv4
  serv_addr.sin_family = AF_INET;

  //Set the address to INADDR_ANY
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  //Set the port number
  serv_addr.sin_port = htons(portno1);

  //Bind the socket
  if(bind(sockfd1, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		cout<<"\nFailed to bind the socket!"<<endl;
		exit(1);
	}

  //Set the option for broadcasting
  if (setsockopt(sockfd1, SOL_SOCKET, SO_BROADCAST, &boolval1, sizeof(boolval1)) < 0)
	{
		cout<<"\nError setting socket options"<<endl;
		exit(1);
	}

  this->sockfd = sockfd1;
  this->portno = portno1;
  this->boolval = boolval1;
  this->serveraddress = serv_addr;

  //Get the ip of the system
  //Pulled from stackoverflow. Modified slightly for my uses
	//https://stackoverflow.com/questions/579783/how-to-detect-ip-address-change-programmatically-in-linux
  struct ifreq ifr;
  strncpy(ifr.ifr_name, "wlan0", sizeof(ifr.ifr_name));
  if(ioctl(this->sockfd, SIOCGIFADDR, &ifr) >= 0){
    this->localAddress = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    cout<<"\nMy IP addr is: "<<this->localAddress<<endl;
  }

}

SocketCommunication::~SocketCommunication(){
  //Close the socket
  close(this->sockfd);
}

int SocketCommunication::sendMessage(char buffer[MSG_SIZE]){
  struct sockaddr_in from1;
  int n;
  //cout<<buffer<<endl;
  //Copy the message over
  strcpy(this->broadcast,buffer);
  //cout<<this->broadcast<<endl;

  //Set the broadcast IP
  from1.sin_addr.s_addr = inet_addr("128.206.19.255");

  //This may throw an error for types.
  n = sendto(this->sockfd, this->broadcast, MSG_SIZE, 0, (struct sockaddr*)&from1, this->fromlen);

  this->fromaddress = from1;
  //Error checking
  if(n < 0){
    cout<<"SEND FAILED"<<endl;
    exit(-1);
  }

  return(1);
}

char* SocketCommunication::receiveMessage(void){
  int n;
  //Clear the receive character array
  bzero(this->receive, MSG_SIZE);

 // n = recvfrom();
  return 0; //just trying to make this compile

}

AnalogInput::AnalogInput() {
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
    if(this->value != OK) this->eventFlag = true;
    cout << this->value; //for debugging
}

DigitalInput::DigitalInput(int pin) {
 //set pull up or pull down if necessary
 //set pin as input
 pinMode(pin,INPUT);
}

void DigitalInput::update() {
  //use wiringpi to check if value on pin is different than value in object
  if(this->value != digitalRead(this->pinNumber));
    this->eventFlag = true; //if theyre different set the event flag
  this->value = digitalRead(this->pinNumber); //always update the value anyways
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

DigitalOutput::DigitalOutput(int pin) {
  this->pinNumber = pin;
  pinMode(this->pinNumber,OUTPUT);
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
