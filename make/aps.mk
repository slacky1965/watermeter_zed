# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/zigbee/aps
 
OBJS += \
$(OUT_PATH)/zigbee/aps/aps_group.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/aps/%.o: $(SDK_PATH)/zigbee/aps/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


