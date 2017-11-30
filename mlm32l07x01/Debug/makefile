################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Projects/SW4STM32/subdir.mk
-include Projects/End_Node/subdir.mk
-include Middlewares/Lora/Utilities/subdir.mk
-include Middlewares/Lora/Mac/Regions/subdir.mk
-include Middlewares/Lora/Mac/subdir.mk
-include Middlewares/Lora/Crypto/subdir.mk
-include Middlewares/Lora/Core/subdir.mk
-include Drivers/STM32L0xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/X_NUCLEO_IKS01A2/subdir.mk
-include Drivers/BSP/MLM32L0X01/subdir.mk
-include Drivers/BSP/Components/subdir.mk
-include B-L072Z-LRWAN1/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: mlm32l07x01.elf

# Tool invocations
mlm32l07x01.elf: $(OBJS) $(USER_OBJS) ../STM32L072CZYx_FLASH.ld
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -specs=nosys.specs -specs=nano.specs -T"../STM32L072CZYx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -o "mlm32l07x01.elf" @"objects.list" $(USER_OBJS) $(LIBS) -lm
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "mlm32l07x01.elf" "mlm32l07x01.bin"
	arm-none-eabi-size "mlm32l07x01.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
