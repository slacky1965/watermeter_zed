# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += /$(SRC_PATH)
 
OBJS += \
$(OUT_PATH)/$(SRC_PATH)/watermeter.o \
$(OUT_PATH)/$(SRC_PATH)/sampleSwitchEpCfg.o \
$(OUT_PATH)/$(SRC_PATH)/zb_appCb.o \
$(OUT_PATH)/$(SRC_PATH)/zcl_sampleSwitchCb.o \
$(OUT_PATH)/$(SRC_PATH)/pulse.o \
$(OUT_PATH)/$(SRC_PATH)/cfg.o \
$(OUT_PATH)/$(SRC_PATH)/button.o \
$(OUT_PATH)/$(SRC_PATH)/battery.o \
$(OUT_PATH)/$(SRC_PATH)/watermeter_zcl.o

#$(OUT_PATH)/$(SRC_PATH)/app_ui.o \
#$(OUT_PATH)/$(SRC_PATH)/sampleSensor.o \
#$(OUT_PATH)/$(SRC_PATH)/sampleSensorEpCfg.o \
#$(OUT_PATH)/$(SRC_PATH)/zb_appCb.o \
#$(OUT_PATH)/$(SRC_PATH)/zcl_sampleSensorCb.o 


#$(OUT_PATH)/$(SRC_PATH)/app_ui.o \
#$(OUT_PATH)/$(SRC_PATH)/sampleSwitch.o \
#$(OUT_PATH)/$(SRC_PATH)/sampleSwitchEpCfg.o \
#$(OUT_PATH)/$(SRC_PATH)/zb_appCb.o \
#$(OUT_PATH)/$(SRC_PATH)/zcl_sampleSwitchCb.o 

#$(OUT_PATH)/$(SRC_PATH)/app_ui.o \
#$(OUT_PATH)/$(SRC_PATH)/sampleSwitch.o \

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/$(SRC_PATH)/%.o: $(SRC_PATH)/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


