################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221/HTS221_Driver.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221/HTS221_Driver_HL.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb/LPS22HB_Driver.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb/LPS22HB_Driver_HL.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb/LPS25HB_Driver.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb/LPS25HB_Driver_HL.c \
/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276/sx1276.c 

OBJS += \
./Drivers/BSP/Components/HTS221_Driver.o \
./Drivers/BSP/Components/HTS221_Driver_HL.o \
./Drivers/BSP/Components/LPS22HB_Driver.o \
./Drivers/BSP/Components/LPS22HB_Driver_HL.o \
./Drivers/BSP/Components/LPS25HB_Driver.o \
./Drivers/BSP/Components/LPS25HB_Driver_HL.o \
./Drivers/BSP/Components/sx1276.o 

C_DEPS += \
./Drivers/BSP/Components/HTS221_Driver.d \
./Drivers/BSP/Components/HTS221_Driver_HL.d \
./Drivers/BSP/Components/LPS22HB_Driver.d \
./Drivers/BSP/Components/LPS22HB_Driver_HL.d \
./Drivers/BSP/Components/LPS25HB_Driver.d \
./Drivers/BSP/Components/LPS25HB_Driver_HL.d \
./Drivers/BSP/Components/sx1276.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/HTS221_Driver.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221/HTS221_Driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/BSP/Components/HTS221_Driver_HL.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221/HTS221_Driver_HL.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/BSP/Components/LPS22HB_Driver.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb/LPS22HB_Driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/BSP/Components/LPS22HB_Driver_HL.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb/LPS22HB_Driver_HL.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/BSP/Components/LPS25HB_Driver.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb/LPS25HB_Driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/BSP/Components/LPS25HB_Driver_HL.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb/LPS25HB_Driver_HL.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Drivers/BSP/Components/sx1276.o: /Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276/sx1276.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -DSTM32L072xx -DUSE_B_L072Z_LRWAN1 -DUSE_HAL_DRIVER -DREGION_EU868 -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Projects/Multi/Applications/LoRa/End_Node/inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/MLM32L07X01" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/STM32L0xx_HAL_Driver/Inc" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Device/ST/STM32L0xx/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/CMSIS/Include" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Crypto" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Phy" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Utilities" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Core" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/Common" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/hts221" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps22hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/lps25hb" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/Components/sx1276" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/X_NUCLEO_IKS01A2" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Drivers/BSP/B-L072Z-LRWAN1" -I"/Users/kevinvdm/Downloads/STM32CubeExpansion_LRWAN_V1.1.2/Middlewares/Third_Party/Lora/Mac/region"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


