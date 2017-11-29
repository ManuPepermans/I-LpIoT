################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LSM303AGR.c \
../src/main.c \
../src/stm32f4xx_hal_msp.c \
../src/stm32f4xx_it.c 

CPP_SRCS += \
../src/LM303AGR.cpp 

OBJS += \
./src/LM303AGR.o \
./src/LSM303AGR.o \
./src/main.o \
./src/stm32f4xx_hal_msp.o \
./src/stm32f4xx_it.o 

C_DEPS += \
./src/LSM303AGR.d \
./src/main.d \
./src/stm32f4xx_hal_msp.d \
./src/stm32f4xx_it.d 

CPP_DEPS += \
./src/LM303AGR.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/include/cmsis/device" -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -DSTM32F401xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"../system/include/cmsis/device" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


