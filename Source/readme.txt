Max Houck and Marshall Lindsay
Fall 2017 ECE 4220 Final Project

To compile and run code for RTU (only works on a raspberry pi)
  Check finalProject.h and make sure that "#define RTU YES" on line 7 IS NOT commented out
  Type "make" in the source folder. This will run the makefile and compile everything in the project.
  Install the kernel module using the following command: "sudo insmod rtukernel.ko"
  Type "dmesg" and follow the instruction to create a character device
  Run "rtu" using the following syntax: "sudo ./rtu 7" The single digit argument will be the devices id that shows up in the historians logs
  RTU will not start logging events until it receives a startup command from the historian
  
To compile and run code for historian
  Check finalProject.h and make sure that "define RTU YES" on line 7 IS commented out. This will allow you to compile the historian on a linux workstation.
  Run "make historian" to only build the historian.
  Run "./historian"
  Enter command '1' to start up RTU's
