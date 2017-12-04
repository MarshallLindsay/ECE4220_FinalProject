/*

Marshall Lindsay
ECE 4220 Lab6 Week 1
Musical Keyboard

HRTIMER Test

*/



#ifndef MODULE
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

MODULE_LICENSE("GPL");

unsigned long middle_c_period 3.8e6; //Interval for middle C in ns. 3.8ms
static struct hrtimer hr_timer;
static int count = 1;


enum hrtimer_restart timer_callback(struct hrtimer *timer_for_restart){
  ktime_t currtime, interval;
  unsigned long overruns = 0;

  currtime = ktimer_get(); //Get the current time
  interval = ktime_set(0, middle_c_period); //Set the end time

  overruns = hrtimer_forward(timer_for_restart, currtime, interval); //Advance the timer to the next interval and return the number of overruns.
  if(overruns > 1 ){
    printk("Too many overrruns in HRtimer module. Overruns: %ld\n", overruns);
  }
  if(count % 2 == 0){
    count--;
    //Clear the pin
  }else{
    count++;
    //Set the pin
  }

  return HRTIMER_RESTART;
}


int timer_init(void){

  ktime_t ktime = ktime_set(0, middle_c_period);

  hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

  hr_timer.function = $timer_callback;

  hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);

  return(0);
}


void timer_exit(void){

  int ret;

  ret = hrtimer_cancel(&hr_timer);

  printk("HR Timer module uninstalling\n");

}

module_init(timer_init);
module_exit(timer_exit);
