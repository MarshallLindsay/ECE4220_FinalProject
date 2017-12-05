/*

Marshall Lindsay
ECE 4220 Lab6 Week 1
Musical Keyboard

KTHREAD Test

*/

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

static struct task_struct *kthread1;

int kthread_fn(void *ptr){
  int count = 1;

  unsigned long j0, j1;

  j0 = jiffies; //Number of clock ticks since the system started.
  j1 = j0 + 10*HZ; //Not sure what this one is

  while(time_before(jiffies, j1)){
    schedule();
  }

  while(1){
    //Check if the thread should be stopped
    if(kthread_should_stop()) {
			do_exit(0);
		}
    usleep_range(3800, 3810); //Not sure how this will work with a tight range
    if(count % 2 == 0){
      count--;
      //Clear the pin
    }else{
      count++;
      //Set the pin
    }
  }

  return(0);
}


int thread_init(void){

  char kthread_name[17] = "musical_keyboard";

  kthread1 = kthread_create(kthread_fn, NULL, kthread_name);

  if((kthread1)){
    wake_up_process(kthread1);
  }

  return(0);
}


void thread_cleanup(void){
  int ret;

  ret = kthread_stop(kthread1);

  if(!ret){
    printk("musical_keyboard Kthread stopped\n");
  }
}

module_init(thread_init);
module_exit(thread_cleanup);