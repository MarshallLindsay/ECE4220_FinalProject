/*
 serial_simple.c
 Created on: Sep 25, 2013
    Author: Luis Alberto Rivera
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serial_ece4220.h"

int main(void)
{
	// Open the serial port. The first argument denotes the serial port number,
	// while the second and third denote the baud rate (of 115200 in this case)
	int prt_id = serial_open(0,5,5);
	unsigned char x;
	ssize_t num_bytes;

	usleep(10);

	while(1)
	{
		//Read data from the serial port. Blocking function
		num_bytes = read(prt_id, &x, 1);
		printf(" [%d,%u] ", (int)num_bytes, x);
		fflush(stdout);
	}
}

