# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += \
/zigbee/wwah

OBJS += \
$(OUT_PATH)/zigbee/wwah/wwah.o \
$(OUT_PATH)/zigbee/wwah/wwahEpCfg.o 
 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/wwah/%.o: $(SDK_PATH)/zigbee/wwah/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

