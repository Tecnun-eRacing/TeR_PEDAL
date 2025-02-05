################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DBC/all.c \
../DBC/charger.c \
../DBC/hvbms.c \
../DBC/inverter.c \
../DBC/ter.c 

OBJS += \
./DBC/all.o \
./DBC/charger.o \
./DBC/hvbms.o \
./DBC/inverter.o \
./DBC/ter.o 

C_DEPS += \
./DBC/all.d \
./DBC/charger.d \
./DBC/hvbms.d \
./DBC/inverter.d \
./DBC/ter.d 


# Each subdirectory must supply rules for building sources it contributes
DBC/%.o DBC/%.su DBC/%.cyclo: ../DBC/%.c DBC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/COLEGIO/OneDrive - SM/Documentos/Tecnun Eracing/TeR_PEDAL/FIRMWARE/TER_PEDAL/DBC" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-DBC

clean-DBC:
	-$(RM) ./DBC/all.cyclo ./DBC/all.d ./DBC/all.o ./DBC/all.su ./DBC/charger.cyclo ./DBC/charger.d ./DBC/charger.o ./DBC/charger.su ./DBC/hvbms.cyclo ./DBC/hvbms.d ./DBC/hvbms.o ./DBC/hvbms.su ./DBC/inverter.cyclo ./DBC/inverter.d ./DBC/inverter.o ./DBC/inverter.su ./DBC/ter.cyclo ./DBC/ter.d ./DBC/ter.o ./DBC/ter.su

.PHONY: clean-DBC

