################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ECE4220_Lab4_EventWatcher.cpp \
../src/serial_ece4220.cpp 

OBJS += \
./src/ECE4220_Lab4_EventWatcher.o \
./src/serial_ece4220.o 

CPP_DEPS += \
./src/ECE4220_Lab4_EventWatcher.d \
./src/serial_ece4220.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


