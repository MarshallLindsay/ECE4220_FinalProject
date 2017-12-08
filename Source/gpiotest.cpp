
#include "finalProject.h"

int main() {
  wiringPiSetup();
  __useconds_t delay = (__useconds_t) 100; //10 uS ms
  
  pinMode(15,OUTPUT); //1 on
  pinMode(16,OUTPUT); //1 on
  
  //rtu should show 6 events
  
  digitalWrite(15,1);
  usleep((__useconds_t)delay);
  digitalWrite(15,0);
  usleep((__useconds_t)delay);
   digitalWrite(15,1);
  usleep((__useconds_t)delay);
  digitalWrite(16,1);
  usleep((__useconds_t)delay);
  digitalWrite(15,0);
  usleep((__useconds_t)delay);
  digitalWrite(16,0);
  usleep((__useconds_t)delay);
 
}
