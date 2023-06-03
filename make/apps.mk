# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/apps/common
 
OBJS += \
$(OUT_PATH)/apps/common/factory_reset.o \
$(OUT_PATH)/apps/common/firmwareEncryptChk.o \
$(OUT_PATH)/apps/common/main.o \
$(OUT_PATH)/apps/common/module_test.o 


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/apps/common/%.o: $(SDK_PATH)/apps/common/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


