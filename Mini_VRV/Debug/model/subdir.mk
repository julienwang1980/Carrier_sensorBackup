################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../model/compressor_model.c \
../model/refrigerant_property.c \
../model/sensor_predict.c 

OBJS += \
./model/compressor_model.o \
./model/refrigerant_property.o \
./model/sensor_predict.o 

C_DEPS += \
./model/compressor_model.d \
./model/refrigerant_property.d \
./model/sensor_predict.d 


# Each subdirectory must supply rules for building sources it contributes
model/%.o model/%.su model/%.cyclo: ../model/%.c model/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../model -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-model

clean-model:
	-$(RM) ./model/compressor_model.cyclo ./model/compressor_model.d ./model/compressor_model.o ./model/compressor_model.su ./model/refrigerant_property.cyclo ./model/refrigerant_property.d ./model/refrigerant_property.o ./model/refrigerant_property.su ./model/sensor_predict.cyclo ./model/sensor_predict.d ./model/sensor_predict.o ./model/sensor_predict.su

.PHONY: clean-model

