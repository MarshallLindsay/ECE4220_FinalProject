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
	int counter = 0;
	int pipe_BtoA, pipe_AtoB;		// for file descriptors
	
	if((pipe_BtoA = open("BtoA", O_WRONLY)) < 0)
	{
		printf("pipe BtoA error\n");
		exit(-1);
	}
	
	if((pipe_AtoB = open("AtoB", O_RDONLY)) < 0)
	{
		printf("pipe AtoB error\n");
		exit(-1);
	}

	while(1)
	{
		sleep(1);
		if(write(pipe_BtoA, &counter, sizeof(counter)) != sizeof(counter))
		{
			printf("BtoA pipe write error\n");
			exit(-1);
		}
		if(read(pipe_AtoB, &counter, sizeof(counter)) < 0)
		{
			printf("AtoB pipe read error\n");
			exit(-1);
		}
		counter++;
	}

	return 0;
}
