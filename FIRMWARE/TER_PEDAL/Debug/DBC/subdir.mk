################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DBC/ter.c 

OBJS += \
./DBC/ter.o 

C_DEPS += \
./DBC/ter.d 


# Each subdirectory must supply rules for building sources it contributes
DBC/%.o DBC/%.su DBC/%.cyclo: ../DBC/%.c DBC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Ozuba/Documents/GitHub/TER_PEDAL/FIRMWARE/TER_PEDAL/DBC" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-DBC

clean-DBC:
	-$(RM) ./DBC/ter.cyclo ./DBC/ter.d ./DBC/ter.o ./DBC/ter.su

.PHONY: clean-DBC

