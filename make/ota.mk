# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += \
/zigbee/ota

OBJS += \
$(OUT_PATH)/zigbee/ota/ota.o \
$(OUT_PATH)/zigbee/ota/otaEpCfg.o 

#$(OUT_PATH)/zigbee/ota/otaCRC.o


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/ota/%.o: $(SDK_PATH)/zigbee/ota/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
