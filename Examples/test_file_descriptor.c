/*
* Written by Luis Rivera

* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, meaning:
* keep this copyright notice,
* do not try to make money out of it,
* it's distributed WITHOUT ANY WARRANTY,
* yada yada yada...
*/

#include <stdio.h>
#include <stdlib.h>

int main()
{
   FILE *pFile;
   char buffer[100], dummy;

   printf("\nBefore opening the file\n");
   fflush(stdout);
   scanf("%c", &dummy);
   
   pFile = fopen("test_file_descriptor.c" , "r");
   if(pFile == NULL)
     perror("Error opening file");
   else
   {
     while(!feof(pFile))
     {
       if(fgets(buffer, 100, pFile) == NULL)
	 break;
       fputs(buffer, stdout);
     }
     
	 // get and display the file descriptor (one of the fields in the structure FILE)
     printf("\n\nBefore closing the file. fd = %d\n", fileno(pFile));
	 fflush(stdout);
     scanf("%c", &dummy);
   
     fclose(pFile);
     
     printf("\nAfter closing the file\n");
	 fflush(stdout);
     scanf("%c", &dummy);
   }
   
   return 0;
}