################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/cmsis/system_stm32l1xx.c 

S_UPPER_SRCS += \
../system/src/cmsis/startup_stm32l152xe.S 

OBJS += \
./system/src/cmsis/startup_stm32l152xe.o \
./system/src/cmsis/system_stm32l1xx.o 

S_UPPER_DEPS += \
./system/src/cmsis/startup_stm32l152xe.d 

C_DEPS += \
./system/src/cmsis/system_stm32l1xx.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/cmsis/%.o: ../system/src/cmsis/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g -x assembler-with-cpp -DNDEBUG -DSTM32L152xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l1xx" -I"../system/include/cmsis/device" -I"/Users/kevinvdm/STM32Toolchain/projects/final/include" -I"/Users/kevinvdm/STM32Toolchain/projects/final/system/include" -I"/Users/kevinvdm/STM32Toolchain/projects/final/system/include/cmsis" -I"/Users/kevinvdm/STM32Toolchain/projects/final/system/include/stm32l1xx" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/src/cmsis/%.o: ../system/src/cmsis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g -DNDEBUG -DSTM32L152xE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l1xx" -I"../system/include/cmsis/device" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


