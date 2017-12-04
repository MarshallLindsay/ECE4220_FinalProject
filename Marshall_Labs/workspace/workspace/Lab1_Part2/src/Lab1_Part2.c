/*
 *
 * Marshall Lindsay
 * ECE 4220
 * Lab1 Part2
 * Kernal Module
 *
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
#include <linux/delay.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
/*
GREEN 8
YELLOW 9
PB1 0
PB2 2
PB3 3
PB4 14
PB5 13
SPEAKER 12
*/
//4009000000

int init_module(void){
	unsigned long* GPFSEL;
	unsigned long* GPSET;

	//unsigned long* GPLEV;
	//unsigned long* GPPUD;
	//unsigned long* GPPUDCLK;

	//int x;

	GPFSEL = (unsigned long*)ioremap(0x3F200000, 4096); //Function Select

	GPSET = (unsigned long*)ioremap(0x3F20001c, 4096); //Set register



	//GPLEV = (unsigned long*)ioremap(0x7E200034, 0x04); //Level register

	//GPPUD = (unsigned long*)ioremap(0x7E200094, 0x2); //Pull up / down control reg

	//GPPUDCLK = (unsigned long*)ioremap(0x7E200094, 0x4); //Pull up / down clock

	*GPFSEL |= 0x1240;	//Set the input/output. Refer to lab1 part1
/*
	//Need to set the pull ups/downs for the correct pins. This is done by
	//"clocking" in the values using the GGPUD and GPPUDCLK

	//Enable the pull ups first. This will be done on all of the push buttons

	//Set the GPPUD control reg

	*GPPUD = 10; //Pull up control

	msleep(1); //Data sheet asks for a wait of 150 cycles. At 1.2GHz thats.. 8.33*10^-10 seconds ... a 1ms delay will work.

	//Clock the pull ups in..

	*GPPUDCLK = 0x0000600D; //This is for all of the push buttons

	msleep(1);

	*GPPUD = 00; //Pull down control

	msleep(1);

	*GPPUDCLK = 0x00000300;	//This is for the yellow and green light


	/*At this point, the push buttons are set as inputs, the lights and speaker are set as outputs.
	 * the push buttons are pulled high and the lights are pulled low.
	 */

	//None of the above was required. This makes me sad.


	printk("MODULE INSTALLED\n");
	*GPSET |= 0x1C; //0001 1100





	return(0);
}

void cleanup_module(void){
	unsigned long* GPFSEL;
	GPFSEL = (unsigned long*)ioremap(0x3F200000, 4096);

	unsigned long* GPCLR;
	GPCLR = (unsigned long*)ioremap(0x3F200028, 4096); //Clear register

	*GPFSEL |= 0x1240;
	*GPCLR |= 0x1C; //0001 1100



	printk("MODULE REMOVED\n");

}
