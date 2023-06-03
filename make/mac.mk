# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += \
/zigbee/mac \

OBJS += \
$(OUT_PATH)/zigbee/mac/mac_phy.o \
$(OUT_PATH)/zigbee/mac/mac_pib.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/mac/%.o: $(SDK_PATH)/zigbee/mac/%.c
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
