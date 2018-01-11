################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LM303.c \
../src/LPS22HB.c \
../src/main.c \
../src/stm32l1xx_hal_msp.c \
../src/stm32l1xx_it.c 

OBJS += \
./src/LM303.o \
./src/LPS22HB.o \
./src/main.o \
./src/stm32l1xx_hal_msp.o \
./src/stm32l1xx_it.o 

C_DEPS += \
./src/LM303.d \
./src/LPS22HB.d \
./src/main.d \
./src/stm32l1xx_hal_msp.d \
./src/stm32l1xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g -DNDEBUG -DSTM32L152xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l1xx" -I"../system/include/cmsis/device" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


