/*
  Marshall Lindsay
  Max Houck
  ECE 4220 Final Project

*/



class DigitalInput{
private:
  int pinNumber;  //What is the pin number?
  bool state;     //What is the state of the input...has an event occurred?
  bool value;     //What is the value on the pin?


public:

  DigitalInput();         //Initialize the input hardware parameters
  ~DigitalInput();        //Destructor
  void setState(bool);    //Set the state if an event happened, or clear if the event is over.
  void setPullUp();       //Set the pullup for the pin.
  void setPullDown();     //Set the pulldown for the pin.
  void setAsInterrupt();  //Set the pin as an interrupt.
  bool getValue();        //Get the value on the pin.
  bool getState();        //Get the state of the pin.
};


class AnalogInput{
private:
  int pinNumber;  //What is the pin number?
  bool state;     //What is the state of the input? Has an event occurred.
  double value;   //What is the value on the pin?

public:
  AnalogInput();        //Initialize the input hardware parameters.
  ~AnalogInput();       //Destructor.
  void setState(bool);  //Set the state if an event happened, or clear if the event is over.
  void setPullUp();      //Set the pullup for the pin.
  void setPullDown();    //Set the pulldown for the pin.
  double getValue();     //Get the value on the pin.
  bool getState();       //Get the state of the pin.
  void setValue();       //Set the value of the pin.
};


class DigitalOutput{
private:
  int pinNumber;  //What is the pin number?
  bool state;     //What is the state on the pin?
  bool value;     //What is the value on the pin?

public:
  DigitalOutput();      //Initialize the input hardware parameters.
  ~DigitalOutput();     //Destructor.
  void setPullUp();     //Set the pullup for the pin.
  void setPullDown();   //Set the pulldown for the pin.
  void setState(bool);  //Set the state if an event happened, or clear if the event is over.
  bool getState();      //Get the state of the pin.
  bool getValue();      //Get the value of the pin.
  void setValue(bool);  //Set the value of the pin.
};
