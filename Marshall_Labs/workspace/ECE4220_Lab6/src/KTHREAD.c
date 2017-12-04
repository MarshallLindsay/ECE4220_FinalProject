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

#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>

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
  unsigned long j0, j1;
  int count = 1;
  int i;
  unsigned long* GPFSEL;
  unsigned long* GPSET;
  unsigned long* GPCLR;

  GPFSEL = (unsigned long*)ioremap(0x3F200000, 4096); //Function Select
  GPSET = GPFSEL;
  GPCLR = GPFSEL;
  *GPFSEL |= 0x41000;	//Set the green led and the speaker as outputs.


  for(i = 0; i < 7; i++){
	  GPSET++;
  }
  for(i = 0; i < 10; i++){
  	  GPCLR++;
  }

  printk("\nStarting the thread");
  printk("\nGPSET = %p   GPCLR = %p", (void*)GPSET, (void*)GPCLR);




  j0 = jiffies; //Number of clock ticks since the system started.
  j1 = j0 + 2*HZ; //Not sure what this one is

  while(time_before(jiffies, j1)){
    schedule();
  }

  while(1){
    //Check if the thread should be stopped
    if(kthread_should_stop()) {
			do_exit(0);
		}
    usleep_range(3800, 3800); //Not sure how this will work with a tight range
    //usleep_range(1000000, 1100000); //Test values
    if(count % 2 == 0){
      count--;
      //printk("\nClearing the pin");
      *GPCLR |= 0x40;
      //Clear the pin
    }else{
      count++;
      //printk("\nSetting the pin");
      *GPSET |= 0x40;
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
