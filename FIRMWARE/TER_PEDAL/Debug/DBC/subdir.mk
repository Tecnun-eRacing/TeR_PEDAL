################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DBC/hvbms.c \
../DBC/inverter.c \
../DBC/te_r23.c \
../DBC/ter.c 

OBJS += \
./DBC/hvbms.o \
./DBC/inverter.o \
./DBC/te_r23.o \
./DBC/ter.o 

C_DEPS += \
./DBC/hvbms.d \
./DBC/inverter.d \
./DBC/te_r23.d \
./DBC/ter.d 


# Each subdirectory must supply rules for building sources it contributes
DBC/%.o DBC/%.su DBC/%.cyclo: ../DBC/%.c DBC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"/home/ozuba/GitHub/TER_PEDAL/FIRMWARE/TER_PEDAL/DBC" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-DBC

clean-DBC:
	-$(RM) ./DBC/hvbms.cyclo ./DBC/hvbms.d ./DBC/hvbms.o ./DBC/hvbms.su ./DBC/inverter.cyclo ./DBC/inverter.d ./DBC/inverter.o ./DBC/inverter.su ./DBC/te_r23.cyclo ./DBC/te_r23.d ./DBC/te_r23.o ./DBC/te_r23.su ./DBC/ter.cyclo ./DBC/ter.d ./DBC/ter.o ./DBC/ter.su

.PHONY: clean-DBC

