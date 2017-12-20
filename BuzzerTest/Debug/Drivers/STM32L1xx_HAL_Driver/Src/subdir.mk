################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_cortex.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_dma.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash_ex.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash_ramfunc.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_gpio.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_pwr.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_pwr_ex.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc_ex.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_tim.c \
../Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_tim_ex.c 

OBJS += \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_cortex.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_dma.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash_ex.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash_ramfunc.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_gpio.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_pwr.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_pwr_ex.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc_ex.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_tim.o \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_tim_ex.o 

C_DEPS += \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_cortex.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_dma.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash_ex.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_flash_ramfunc.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_gpio.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_pwr.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_pwr_ex.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_rcc_ex.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_tim.d \
./Drivers/STM32L1xx_HAL_Driver/Src/stm32l1xx_hal_tim_ex.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/STM32L1xx_HAL_Driver/Src/%.o: ../Drivers/STM32L1xx_HAL_Driver/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32L152xE -I"/Users/kevinvdm/Desktop/iwdgtest/BuzzerTest/Inc" -I"/Users/kevinvdm/Desktop/iwdgtest/BuzzerTest/Drivers/STM32L1xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Desktop/iwdgtest/BuzzerTest/Drivers/STM32L1xx_HAL_Driver/Inc/Legacy" -I"/Users/kevinvdm/Desktop/iwdgtest/BuzzerTest/Drivers/CMSIS/Device/ST/STM32L1xx/Include" -I"/Users/kevinvdm/Desktop/iwdgtest/BuzzerTest/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


