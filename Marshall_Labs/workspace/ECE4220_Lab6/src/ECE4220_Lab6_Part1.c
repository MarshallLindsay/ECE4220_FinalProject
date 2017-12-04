/*
 * Marshall Lindsay
 * ECE4220 Lab6 Part1
 * Musical Keyboard
 *
 */

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kthread.h>	// for kthreads
#include <linux/sched.h>	// for task_struct
#include <linux/time.h>		// for using jiffies

#include <linux/fs.h>
#include <asm/uaccess.h>

#define MSG_SIZE 40
#define CDEV_NAME "MarshallLab6"

MODULE_LICENSE("GPL");

static int major;
static char msg[MSG_SIZE];
static char send_msg[MSG_SIZE];
int button_semaphore;

/* Declare your pointers for mapping the necessary GPIO registers.
   You need to map:

   - Pin Event detect status register(s)
   - Rising Edge detect register(s) (either synchronous or asynchronous should work)
   - Function selection register(s)
   - Pin Pull-up/pull-down configuration registers

   Important: remember that the GPIO base register address is 0x3F200000, not the
   one shown in the BCM2835 ARM Peripherals manual.
*/

unsigned long period = 2272; //Interval for HRTIMER.
static struct task_struct *kthread1;

unsigned long* GPFSEL;	//Select register. Set as inputs and outputs.
unsigned long* GPSET;	//GPIO set register. Used to set a pin.
unsigned long* GPCLR;	//GPIO clear register. Used to clear a pin.
unsigned long* GPEDS;	//GPIO Event Detect Status. Used to see which pin was set.
unsigned long* GPAREN;	//GPIO Async. Rising Edge Detection enable register.
unsigned long* GPPUD;	//GPIO Pullup/Pulldown enable.
unsigned long* GPPUDCLK;//GPPUD clock for Pullup/Pulldown configuration.



int mydev_id;	// variable needed to identify the handler

static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
	if(button_semaphore){
		// Whatever is in msg will be placed into buffer, which will be copied into user space
		ssize_t dummy = copy_to_user(buffer, send_msg, length);	// dummy will be 0 if successful

		// msg should be protected (e.g. semaphore). Not implemented here, but you can add it.
		send_msg[0] = '\0';	// "Clear" the message, in case the device is read again.
						// This way, the same message will not be read twice.
					// Also convenient for checking if there is nothing new, in user space.
		button_semaphore--;
	}
	return length;
}


static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
	ssize_t dummy;

	if(len > MSG_SIZE){
		printk("\nWeird return");
		return -EINVAL;
	}

	// unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
	dummy = copy_from_user(msg, buff, len);	// Transfers the data from user space to kernel space
	if(len == MSG_SIZE)
		msg[len-1] = '\0';	// will ignore the last character received.
	else
		msg[len] = '\0';

	// You may want to remove the following printk in your final version.
	printk("Message from user space: %s\n", msg);

	if(strcmp(msg, "@A") == 0){
		period = 2272;
	}else if(strcmp(msg, "@B") == 0){
		period = 2025;
	}else if(strcmp(msg, "@C") == 0){
		period = 1912;
	}else if(strcmp(msg, "@D") == 0){
		period = 1703;
	}else if(strcmp(msg, "@E") == 0){
		period = 1517;
	}

	return len;		// the number of bytes that were written to the Character Device.
}

// structure needed when registering the Character Device. Members are the callback
// functions when the device is read from or written to.
static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
};




// Interrupt handler function. Tha name "button_isr" can be different.
// You may use printk statements for debugging purposes. You may want to remove
// them in your final code.
static irqreturn_t button_isr(int irq, void *dev_id)
{
	// In general, you want to disable the interrupt while handling it.
	disable_irq_nosync(79);

	// This same handler will be called regardless of what button was pushed,
	// assuming that they were properly configured.
	// How can you determine which button was the one actually pushed?
	if(*GPEDS & 0x10000){
		printk("\nPB1 was pressed");
		period = 2272;
		strcpy("@A", send_msg);
		button_semaphore++;
	}else if(*GPEDS & 0x20000){
		printk("\nPB2 was pressed");
		period = 2025;
		strcpy("@B", send_msg);
		button_semaphore++;
	}else if(*GPEDS & 0x40000){
		printk("\nPB3 was pressed");
		period = 1912;
		strcpy("@C", send_msg);
		button_semaphore++;
	}else if(*GPEDS & 0x80000){
		printk("\nPB4 was pressed");
		period = 1703;
		strcpy("@D", send_msg);
		button_semaphore++;
	}else if(*GPEDS & 0X100000){
		printk("\nPB5 was pressed");
		period = 1517;
		strcpy("@E", send_msg);
		button_semaphore++;
	}

	// DO STUFF (whatever you need to do, based on the button that was pushed)

	// IMPORTANT: Clear the Event Detect status register before leaving.

	*GPEDS = 0xFFFFFFFF;
	printk("Interrupt handled\n");
	enable_irq(79);		// re-enable interrupt

    return IRQ_HANDLED;
}

int kthread_fn(void *ptr){
  unsigned long j0, j1;
  int count = 1;

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
    usleep_range(period, period); //Not sure how this will work with a tight range
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

int init_module()
{
	int dummy = 0;
	int i;
	char kthread_name[17] = "musical_keyboard";

	// Map GPIO registers
	// Remember to map the base address (beginning of a memory page)
	// Then you can offset to the addresses of the other registers

	GPFSEL = (unsigned long*)ioremap(0x3F200000, 4096);
	GPSET = GPFSEL;		//7
	GPCLR = GPFSEL;		//10
	GPEDS = GPFSEL;		//16
	GPAREN = GPFSEL;	//31
	GPPUD = GPFSEL;		//37
	GPPUDCLK = GPFSEL;	//38

	for(i = 0; i < 7; i++){
	 	GPSET++;
	}
	for(i = 0; i < 10; i++){
		GPCLR++;
	}
	for(i = 0; i < 16; i++){
		GPEDS++;
	}
	for(i = 0; i < 31; i++){
		GPAREN++;
	}
	for(i = 0; i < 37; i++){
		GPPUD++;
	}
	for(i = 0; i < 38; i++){
		GPPUDCLK++;
	}

	printk("\nGPSET = %p | GPCLR = %p | GPEDS = %p | GPAREN = %p | GPPUD = %p | GPPUDCLK = %p", (void*)GPSET, (void*)GPCLR, (void*)GPEDS, (void*)GPAREN, (void*)GPPUD, (void*)GPPUDCLK);

	// Don't forget to configure all ports connected to the push buttons as inputs.

	//Speaker - GPIO6
	//PB1 - GPIO16
	//PB2 - GPIO17
	//PB3 - GPIO18
	//PB4 - GPIO19
	//PB5 - GPIO20

	//Speaker
	*GPFSEL |= 0x40000;

	//Push buttons 1 - 4 are on the next register
	GPFSEL++;
	//PB1, PB2, PB3, PB4
	*GPFSEL &= 0xF3DBFFFF;

	//Push button 5 is on the next register
	GPFSEL++;
	//PB5
	*GPFSEL &= 0xFFFFFFF8;



	// You need to configure the pull-downs for all those ports. There is
	// a specific sequence that needs to be followed to configure those pull-downs.
	// The sequence is described on page 101 of the BCM2835-ARM-Peripherals manual.
	// You can use  udelay(100);  for those 150 cycles mentioned in the manual.
	// It's not exactly 150 cycles, but it gives the necessary delay.
	// WiringPi makes it a lot simpler in user space...

	//Pull downs for bits 15,16,17,18,19 ->
	*GPPUD |= 0x1;	//0b01
	udelay(100);
	*GPPUDCLK |= 0x1F0000;
	udelay(100);
	*GPPUD &= 0x0;
	udelay(100);
	*GPPUDCLK &= 0x0;
	// Enable (Async) Rising Edge detection for all 5 GPIO ports.
	*GPAREN |= 0x1F0000;

	// Request the interrupt / attach handler (line 79, doesn't match the manual...)
	// The third argument string can be different (you give the name)
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	kthread1 = kthread_create(kthread_fn, NULL, kthread_name);

	if((kthread1)){
	  wake_up_process(kthread1);
	}


	printk("Button Detection enabled.\n");

	// register the Characted Device and obtain the major (assigned by the system)
	major = register_chrdev(0, CDEV_NAME, &fops);
	if (major < 0) {
		printk("Registering the character device failed with %d\n", major);
		return major;
	}

	printk("Create Char Device (node) with: sudo mknod /dev/%s c %d 0\n", CDEV_NAME, major);

	return 0;
}

// Cleanup - undo whatever init_module did
void cleanup_module()
{
    int ret;
	// Good idea to clear the Event Detect status register here, just in case.
	*GPEDS |= 0xFFFFFFFF;
	// Disable (Async) Rising Edge detection for all 5 GPIO ports.
	*GPAREN |= 0x0;
	// Remove the interrupt handler; you need to provide the same identifier
    free_irq(79, &mydev_id);

    ret = kthread_stop(kthread1);

    if(!ret){
      printk("musical_keyboard Kthread stopped\n");
    }

	printk("Button Detection disabled.\n");

	unregister_chrdev(major, CDEV_NAME);
	printk("Char device /dev/%s unregistered.\n", CDEV_NAME);
}
