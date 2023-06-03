# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/zigbee/af
 
OBJS += \
$(OUT_PATH)/zigbee/af/zb_af.o 


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/af/%.o: $(SDK_PATH)/zigbee/af/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


