# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/zigbee/zdo
 
OBJS += \
$(OUT_PATH)/zigbee/zdo/zdp.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/zdo/%.o: $(SDK_PATH)/zigbee/zdo/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


