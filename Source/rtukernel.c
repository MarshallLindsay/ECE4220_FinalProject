/* Lab6_cdev_kmod.c
 * ECE4220/7220
 * Based on code from: https://github.com/blue119/kernel_user_space_interfaces_example/blob/master/cdev.c
 * Modified and commented by: Luis Alberto Rivera
  
 You can compile the module using the Makefile provided in week 1. Just add
 obj-m += ece4220lab6_cdev.o
 
 This Kernel module prints its "MajorNumber" to the system log. The "MinorNumber"
 can be chosen to be 0.
  
 -----------------------------------------------------------------------------------
 Broadly speaking: The Major Number refers to a type of device/driver, and the
 Minor Number specifies a particular device of that type or sometimes the operation
 mode of that device type. On a terminal, try:
    ls -l /dev/
 You'll see a list of devices, with two numbers (among other pieces of info). For
 example, you'll see tty0, tty1, etc., which represent the terminals. All those have
 the same Major number, but they will have different Minor numbers: 0, 1, etc.
 -----------------------------------------------------------------------------------
  
 After installing the module, 
  
 1) Check the MajorNumber using dmesg
  
 2) You can then create a Character device using the MajorNumber returned:
      sudo mknod /dev/YourDevName c MajorNumber 0
    You need to create the device every time the Pi is rebooted.
     
 3) Change writing permissions, so that everybody can write to the Character Device:
      sudo chmod go+w /dev/YourDevName
    Reading permissions should be enabled by default. You can check using
      ls -l /dev/YourDevName
    You should see: crw-rw-rw-
 
 After the steps above, you will be able to use the Character Device.
 If you uninstall your module, you won't be able to access your Character Device.
 If you install it again (without having shutdown the Pi), you don't need to
 create the device again --steps 2 and 3--, unless you manually delete it.
  
 Note: In this implementation, there is no buffer associated to writing to the
 Character Device. Every new string written to it will overwrite the previous one.
*/
   
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif
 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>    // for kthreads
 
#include <linux/sched.h>  // for task_struct
 
#include <linux/time.h>       // for using jiffies 
 
#include <linux/timer.h>
 
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
 
#define MSG_SIZE 40
#define CDEV_NAME "RTU"    // "YourDevName"
 
MODULE_LICENSE("GPL");
  
static int major; 
static char msg[MSG_SIZE];
static struct task_struct *kthread1;
int mydev_id;
//u_int32_t *gpeds;
u_int32_t *gpset;
u_int32_t *gpclr;
//char note;
//int delay;
 /*
static irqreturn_t button_isr(int irq, void *dev_id) {
  //disable interrupt handlng
    disable_irq_nosync(79);
  //do stuff
      //read event detect status
     printk("button press detected");
     if(*gpeds == (1 << 16)) 
          note = 'C';     
      if(*gpeds == (1 << 17)) 
          note = 'G';     
      if(*gpeds == (1 << 18)) 
          note = 'D';     
      if(*gpeds == (1 << 19)) 
          note = 'A';     
      if(*gpeds == (1 << 20)) 
          note = 'E';     
     switch(note) {
     case 'C':
       delay = 400;
       break;
     case 'G':
          delay = 450;
         break;
     case 'D':
          delay = 300;
       break;
     case 'A':
            delay = 568;
       break;
     case 'E':
            delay = 379;
            break;
    case 'x':
            delay = 0;
       break;
       }
   
  //clear interrupt
      *gpeds = ~0; //set everything to 1 to clear register
   
  //re-enable interrupt handling
  printk("Interrupt handled\n");    
    enable_irq(79);     // re-enable interrupt
     
  
  
    return IRQ_HANDLED;
} */
 /*
int kthread_fn(void *ptr)
 
{
     
 
    unsigned long j0, j1;
 
    int count = 0;
 
 
 
    printk("In kthread1\n");
 
    j0 = jiffies;       // number of clock ticks since system started;
 
                        // current "time" in jiffies
 
    j1 = j0 + 10*HZ;    // HZ is the number of ticks per second, that is
 
                        // 1 HZ is 1 second in jiffies
 
     
 
    while(time_before(jiffies, j1)) // true when current "time" is less than j1
 
        schedule();     // voluntarily tell the scheduler that it can schedule
 
                        // some other process
 
     
 
    printk("Before loop\n");
 
     
 
    // The kthread does not need to run forever. It can execute something
 
    // and then leave.
 
    while(1)
 
    {
 
         *gpset |= (1 << 6); //set pin 0 high
            usleep_range(delay,delay);  // good for > 10 ms
       *gpclr |= (1<<6); //clear pin 0
        usleep_range(delay,delay);  // good for > 10 ms
     
 
        //msleep_interruptible(1000); // good for > 10 ms
 
        //udelay(unsigned long usecs);  // good for a few us (micro s)
 
        //usleep_range(unsigned long min, unsigned long max); // good for 10us - 20 ms
 
         
 
         
 
        // In an infinite loop, you should check if the kthread_stop
 
        // function has been called (e.g. in clean up module). If so,
 
        // the kthread should exit. If this is not done, the thread
 
        // will persist even after removing the module.
 
        if(kthread_should_stop()) {
  //    iounmap(gpset);
 //     iounmap(gpclr);
            do_exit(0);
 
        }
 
                 
 
        // comment out if your loop is going "fast". You don't want to
 
        // printk too often. Sporadically or every second or so, it's okay.
 
//      printk("Count: %d\n", ++count);
 
    }
 
     
 
    return 0;
 
} */
 
 
// Function called when the user space program reads the character device.
// Some arguments not used here.
// buffer: data will be placed there, so it can go to user space
// The global variable msg is used. Whatever is in that string will be sent to userspace.
// Notice that the variable may be changed anywhere in the module...
static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
    // Whatever is in msg will be placed into buffer, which will be copied into user space
//  ssize_t dummy = copy_to_user(buffer, msg, length);  // dummy will be 0 if successful
 
    // msg should be protected (e.g. semaphore). Not implemented here, but you can add it.
//  msg[0] = '\0';  // "Clear" the message, in case the device is read again.
                    // This way, the same message will not be read twice.
                    // Also convenient for checking if there is nothing new, in user space.
     
    return length;
} 
 
// Function called when the user space program writes to the Character Device.
// Some arguments not used here.
// buff: data that was written to the Character Device will be there, so it can be used
//       in Kernel space.
// In this example, the data is placed in the same global variable msg used above.
// That is not needed. You could place the data coming from user space in a different
// string, and use it as needed...
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
    ssize_t dummy;
     
    if(len > MSG_SIZE)
        return -EINVAL;
     
    // unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
    dummy = copy_from_user(msg, buff, len); // Transfers the data from user space to kernel space
    if(len == MSG_SIZE)
        msg[len-1] = '\0';  // will ignore the last character received.
    else
        msg[len] = '\0';
     
    // You may want to remove the following printk in your final version.
    printk("Message from user space: %s\n", msg);
     
    switch(msg[0]) {
		case 1:
			if(msg[1] == 1)
				*gpset |= (1 << 2);
			if(msg[1] == 0)
				*gpclr |= (1 << 2);
			break;
		case 2:
			if(msg[1] == 1)
				*gpset |= (1 << 3);
			if(msg[1] == 0)
				*gpclr |= (1 << 3);
			break;
		case 3:
			if(msg[1] == 1)
				*gpset |= (1 << 4);
			if(msg[1] == 0)
				*gpclr |= (1 << 4);
			break;
		default: break;
	}
    
    return len;     // the number of bytes that were written to the Character Device.
}
 
// structure needed when registering the Character Device. Members are the callback
// functions when the device is read from or written to.
static struct file_operations fops = {
    .read = device_read, 
    .write = device_write,
};
 
int cdev_module_init(void)
{
    // register the Characted Device and obtain the major (assigned by the system)
    major = register_chrdev(0, CDEV_NAME, &fops);
    if (major < 0) {
            printk("Registering the character device failed with %d\n", major);
            return major;
    }
    printk("Lab6_cdev_kmod example, assigned major: %d\n", major);
    printk("Create Char Device (node) with: sudo mknod /dev/%s c %d 0\n", CDEV_NAME, major);
     
 //     delay = 1911;
  //      char kthread_name[11]="my_kthread"; // try running  ps -ef | grep my_kthread
 
                                            // when the thread is active.
 
  //      printk("In init module\n");
 
 //       printk("break 1");
      //gpio setup
      //will use gpio26 (pin 19)
      //make sure you change the gpfsel register if a different pin is used
        // set up speaker gpio
         u_int32_t *gpfsel = ioremap(0x3F200000,32); //gpfsel0
      
              *gpfsel |= (001 << (2*3)); //function select 001 is output for LEDS
              *gpfsel |= (001 << (3*3)); 
              *gpfsel |= (001 << (4*3)); 
    //      printk("%d\r\n",gpfsel);
       //   gpfsel += 0x4 / 4; //gpfsel1
    //          printk("%d\r\n",gpfsel);
    //        *gpfsel = 0; //buttons are all 000 inputs
    //        gpfsel += 0x4 / 4; //gpfsel2
   //         *gpfsel = 0;
  //          gpfsel -= 0x8 / 4;
            //iounmap(gpfsel);
 /*        u_int32_t *gppud = gpfsel+0x94 / 4;//ioremap(0x3F200094,32);
           *gppud = 01;   //gppud to 01 for pull down control
         u_int32_t *gppudclk = gpfsel+0x98 / 4;//ioremap(0x3F200098,32); //gppudclk0
           udelay(100); // //wait 150 cycles. msleep is good for > 10 ms
           *gppudclk |= ((1<<16) | (1<<17) | (1<<18) | (1<<19) | (1<<20));
            udelay(100);    // //wait 150 cycles. msleep is good for > 10 ms
            *gppud = 0; //clear gppud
            *gppudclk = 0; //clear gppudclk
        //        iounmap(gppud);
        //   iounmap(gppudclk);
            uint32_t *gpfen = gpfsel+0x58 / 4;
            *gpfen |= ((1<<16) | (1<<17) | (1<<18) | (1<<19) | (1<<20));
           printk("break 2");
         gpeds = gpfsel+0x40 / 4;//ioremap(0x3F200040,32); //gpeds0 */
 
          gpset = gpfsel+0x1C / 4;//ioremap(0x3F20001C,32); //gpset0
         gpclr = gpfsel+0x28 / 4;//ioremap(0x3F200028,32); //gpclr0     
          
          /*
         int dummy = 0;
             dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
                //clear interrupt
             //enable interrupt handling
             enable_irq(79);
 
        kthread1 = kthread_create(kthread_fn, NULL, kthread_name);*/
 
         
 /*
        if((kthread1))  // true if kthread creation is successful
 
        {
 
            printk("Inside if\n");
 
            // kthread is dormant after creation. Needs to be woken up
 
            wake_up_process(kthread1);
 
        }*/
 
    return 0;
}
 
void cdev_module_exit(void)
{
    // Once unregistered, the Character Device won't be able to be accessed,
    // even if the file /dev/YourDevName still exists. Give that a try...
    unregister_chrdev(major, CDEV_NAME);
    printk("Char Device /dev/%s unregistered.\n", CDEV_NAME);
     
 //   int ret;
 
        // the following doesn't actually stop the thread, but signals that
 
        // the thread should stop itself (with do_exit above).
 
        // kthread should not be called if the thread has already stopped.
  //  free_irq(79, &mydev_id);
 //       ret = kthread_stop(kthread1);
 
}  
 
module_init(cdev_module_init);
module_exit(cdev_module_exit);
