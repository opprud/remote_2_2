################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/RF22.c \
../src/led.c \
../src/m0utils.c \
../src/main.c \
../src/spi.c \
../src/trigInput.c 

OBJS += \
./src/RF22.o \
./src/led.o \
./src/m0utils.o \
./src/main.o \
./src/spi.o \
./src/trigInput.o 

C_DEPS += \
./src/RF22.d \
./src/led.d \
./src/m0utils.d \
./src/main.d \
./src/spi.d \
./src/trigInput.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -D__USE_CMSIS -DDEBUG -D__CODE_RED -I../cmsis -I../config -I../driver -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


