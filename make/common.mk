# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/zigbee/common
 
OBJS += \
$(OUT_PATH)/zigbee/common/zb_config.o \
$(OUT_PATH)/zigbee/common/zb_version.o



# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/common/%.o: $(SDK_PATH)/zigbee/common/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


