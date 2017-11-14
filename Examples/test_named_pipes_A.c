/*
* Copyright (C) 2005-2017 ViGIR Vision-Guided and Intelligent Robotics Lab
* Written by Guilherme DeSouza, Luis Rivera
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, meaning:
* keep this copyright notice,
* do not try to make money out of it,
* it's distributed WITHOUT ANY WARRANTY,
* yada yada yada...
*/

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int counter, dummy;
	int pipe_BtoA, pipe_AtoB;		// for file descriptors
	
	dummy = system("mkfifo BtoA"); 	// could be done separetely in each task,
	dummy = system("mkfifo AtoB");	// or in a terminal

	if((pipe_BtoA = open("BtoA", O_RDONLY)) < 0)
	{
		printf("pipe BtoA error\n");
		exit(-1);
	}
	
	if((pipe_AtoB = open("AtoB", O_WRONLY)) < 0)
	{
		printf("pipe AtoB error\n");
		exit(-1);
	}

	while(1)
	{
		sleep(1);
		if(read(pipe_BtoA, &counter, sizeof(counter)) < 0)
		{
			printf("BtoA pipe read error\n");
			exit(-1);
		}
		counter++;
		
		if(write(pipe_AtoB, &counter, sizeof(counter)) != sizeof(counter))
		{
			printf("AtoB pipe write error\n");
			exit(-1);
		}
		printf("Count TaskA: %d \n", counter);
	}

	return dummy;
}
