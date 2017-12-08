obj-m += rtukernel.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	g++ -lwiringPi -lpthread finalProject.cpp finalProject.h rtu.cpp -o rtu -Wall -std=c++11
	g++ -lwiringPi -lpthread finalProject.cpp finalProject.h historian.cpp -o historian -Wall -std=c++11
	g++ -lwiringPi -lpthread finalProject.cpp finalProject.h historianTest.cpp -o historianTest -Wall -std=c++11 
rtu: 
	g++ -lwiringPi -lpthread finalProject.cpp finalProject.h rtu.cpp -o rtu -Wall -std=c++11
historian:
	g++ -pthread -lpthread -lm -lrt finalProject.cpp finalProject.h historian.cpp -o historian -Wall -std=c++11
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
