# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += \
/zigbee/gp

OBJS += \
$(OUT_PATH)/zigbee/gp/gp.o \
$(OUT_PATH)/zigbee/gp/gpEpCfg.o \
$(OUT_PATH)/zigbee/gp/gp_proxy.o \
$(OUT_PATH)/zigbee/gp/gp_proxyTab.o \
$(OUT_PATH)/zigbee/gp/gp_sink.o \
$(OUT_PATH)/zigbee/gp/gp_sinkTab.o \
$(OUT_PATH)/zigbee/gp/gp_trans.o \
$(OUT_PATH)/zigbee/gp/gp_transCfg.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zigbee/gp/%.o: $(SDK_PATH)/zigbee/gp/%.c
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
