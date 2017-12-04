################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/Lab1_Part2.mod.o \
../src/Lab1_Part2.o 

C_SRCS += \
../src/Lab1_Part2.c \
../src/Lab1_Part2.mod.c 

OBJS += \
./src/Lab1_Part2.o \
./src/Lab1_Part2.mod.o 

C_DEPS += \
./src/Lab1_Part2.d \
./src/Lab1_Part2.mod.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


