# Add inputs and outputs from these tool invocations to the build variables
OUT_DIR += \
/zbhci \
/zbhci/usbPrint \
/zbhci/usbHID \
/zbhci/usbCDC \
/zbhci/uart
 
OBJS += \
$(OUT_PATH)/zbhci/zbhci.o \
$(OUT_PATH)/zbhci/zbhciCmdProcess.o \
$(OUT_PATH)/zbhci/zbhci_zclHandler.o \
$(OUT_PATH)/zbhci/usbPrint/hci_usbPrint.o \
$(OUT_PATH)/zbhci/usbHID/zbhci_usb_hid.o \
$(OUT_PATH)/zbhci/usbCDC/zbhci_usb_cdc.o \
$(OUT_PATH)/zbhci/uart/hci_uart.o 



# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/zbhci/%.o: $(SDK_PATH)/zbhci/%.c 
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


