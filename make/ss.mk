# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += \
/zigbee/ss

OBJS += \
$(OUT_PATH)/zigbee/ss/ss_nv.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/ss/%.o: $(SDK_PATH)/zigbee/ss/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

