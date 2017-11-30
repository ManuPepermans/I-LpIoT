################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities/delay.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities/low_power.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities/timeServer.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities/utilities.c 

OBJS += \
./Middlewares/Lora/Utilities/delay.o \
./Middlewares/Lora/Utilities/low_power.o \
./Middlewares/Lora/Utilities/timeServer.o \
./Middlewares/Lora/Utilities/utilities.o 

C_DEPS += \
./Middlewares/Lora/Utilities/delay.d \
./Middlewares/Lora/Utilities/low_power.d \
./Middlewares/Lora/Utilities/timeServer.d \
./Middlewares/Lora/Utilities/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Lora/Utilities/delay.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities/delay.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/Lora/Utilities/low_power.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities/low_power.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/Lora/Utilities/timeServer.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities/timeServer.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Middlewares/Lora/Utilities/utilities.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities/utilities.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

