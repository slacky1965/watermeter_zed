# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/zigbee/bdb
 
OBJS += \
$(OUT_PATH)/zigbee/bdb/bdb.o 


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/bdb/%.o: $(SDK_PATH)/zigbee/bdb/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


