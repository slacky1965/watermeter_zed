# Set Project Name
PROJECT_NAME := watermeter_zed

# Set the serial port number for downloading the firmware
DOWNLOAD_PORT := COM3

COMPILE_PREFIX = C:/TelinkSDK/opt/tc32/bin/tc32

AS      = $(COMPILE_PREFIX)-elf-as
CC      = $(COMPILE_PREFIX)-elf-gcc
LD      = $(COMPILE_PREFIX)-elf-ld
NM      = $(COMPILE_PREFIX)-elf-nm
OBJCOPY = $(COMPILE_PREFIX)-elf-objcopy
OBJDUMP = $(COMPILE_PREFIX)-elf-objdump
ARCH	= $(COMPILE_PREFIX)-elf-ar
SIZE	= $(COMPILE_PREFIX)-elf-size

LIBS := -lzb_ed -ldrivers_8258

DEVICE_TYPE = -DEND_DEVICE=1
MCU_TYPE = -DMCU_CORE_8258=1
BOOT_FLAG = -DMCU_CORE_8258 -DMCU_STARTUP_8258

SDK_PATH := ./tl_zigbee_sdk
SRC_PATH := ./src
OUT_PATH := ./out
MAKE_INCLUDES := ./make
TOOLS_PATH := ./tools
#VERSION_RELEASE := V$(shell awk -F " " '/APP_RELEASE/ {gsub("0x",""); printf "%d", $$3/10; exit}' $(SRC_PATH)/include/version_cfg.h)
VERSION_RELEASE := V$(shell awk -F " " '/APP_RELEASE/ {gsub("0x",""); printf "%.1f", $$3/10.0; exit}' $(SRC_PATH)/include/version_cfg.h)
VERSION_BUILD := $(shell awk -F " " '/APP_BUILD/ {gsub("0x",""); printf "%02d", $$3; exit}' ./src/include/version_cfg.h)
ZCL_VERSION_FILE := $(shell git log -1 --format=%cd --date=format:%Y%m%d -- src |  sed -e "'s/./\'&\',/g'" -e "'s/.$$//'")


TL_CHECK = $(TOOLS_PATH)/tl_check_fw.py
MAKE_OTA = $(TOOLS_PATH)/make_ota.py

INCLUDE_PATHS := \
-I$(SDK_PATH)/platform \
-I$(SDK_PATH)/proj/common \
-I$(SDK_PATH)/proj \
-I$(SDK_PATH)/zigbee/common/includes \
-I$(SDK_PATH)/zigbee/zbapi \
-I$(SDK_PATH)/zigbee/bdb/includes \
-I$(SDK_PATH)/zigbee/gp \
-I$(SDK_PATH)/zigbee/zcl \
-I$(SDK_PATH)/zigbee/ota \
-I$(SDK_PATH)/zbhci \
-I$(SRC_PATH) \
-I$(SRC_PATH)/include \
-I$(SRC_PATH)/common \
-I$(SRC_PATH)/zcl \
-I./common
 

LS_FLAGS := $(SDK_PATH)/platform/boot/8258/boot_8258.link

GCC_FLAGS := \
-ffunction-sections \
-fdata-sections \
-Wall \
-O2 \
-fpack-struct \
-fshort-enums \
-finline-small-functions \
-std=gnu99 \
-fshort-wchar \
-fms-extensions

GCC_FLAGS += \
$(DEVICE_TYPE) \
$(MCU_TYPE)

ifeq ($(strip $(ZCL_VERSION_FILE)),)
GCC_FLAGS += \
-DBUILD_DATE="{8,'2','0','2','3','1','1','1','7'}"
else
GCC_FLAGS += \
-DBUILD_DATE="{8,$(ZCL_VERSION_FILE)}"
endif
  
OBJ_SRCS := 
S_SRCS := 
ASM_SRCS := 
C_SRCS := 
S_UPPER_SRCS := 
O_SRCS := 
FLASH_IMAGE := 
ELFS := 
OBJS := 
LST := 
SIZEDUMMY := 


RM := rm -rf

# All of the sources participating in the build are defined here
-include $(MAKE_INCLUDES)/zdo.mk
-include $(MAKE_INCLUDES)/zcl.mk
-include $(MAKE_INCLUDES)/wwah.mk
-include $(MAKE_INCLUDES)/ss.mk
-include $(MAKE_INCLUDES)/ota.mk
-include $(MAKE_INCLUDES)/mac.mk
-include $(MAKE_INCLUDES)/gp.mk
-include $(MAKE_INCLUDES)/common.mk
-include $(MAKE_INCLUDES)/bdb.mk
-include $(MAKE_INCLUDES)/aps.mk
-include $(MAKE_INCLUDES)/af.mk
-include $(MAKE_INCLUDES)/zbhci.mk
-include $(MAKE_INCLUDES)/proj.mk
-include $(MAKE_INCLUDES)/platformS.mk
-include $(MAKE_INCLUDES)/div_mod.mk
-include $(MAKE_INCLUDES)/platform.mk
-include ./project.mk

# Add inputs and outputs from these tool invocations to the build variables 
LST_FILE := $(OUT_PATH)/$(PROJECT_NAME).lst
BIN_FILE := $(OUT_PATH)/$(PROJECT_NAME).bin
ELF_FILE := $(OUT_PATH)/$(PROJECT_NAME).elf

SIZEDUMMY += \
sizedummy \


# All Target
all: pre-build main-build

flash: $(BIN_FILE)
	@python3 $(TOOLS_PATH)/TlsrPgm.py -p$(DOWNLOAD_PORT) -z11 -a-100 -s -m we 0 $(BIN_FILE)
	
erase-flash:
	@python3 $(TOOLS_PATH)/TlsrPgm.py -p$(DOWNLOAD_PORT) -z11 -a-100 -s ea

reset:
	@python3 $(TOOLS_PATH)/TlsrPgm.py -p$(DOWNLOAD_PORT) -z11 -a-100 -s -t50 -a2550 -m -w i


# Main-build Target
main-build: clean-project $(ELF_FILE) secondary-outputs

# Tool invocations
$(ELF_FILE): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: TC32 C Linker'
	$(LD) --gc-sections -L $(SDK_PATH)/zigbee/lib/tc32 -L $(SDK_PATH)/platform/lib -T $(LS_FLAGS) -o "$(ELF_FILE)" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	

$(LST_FILE): $(ELF_FILE)
	@echo 'Invoking: TC32 Create Extended Listing'
	$(OBJDUMP) -x -D -l -S $(ELF_FILE)  > $(LST_FILE)
	@echo 'Finished building: $@'
	@echo ' '

$(BIN_FILE): $(ELF_FILE)
	@echo 'Create Flash image (binary format)'
	@$(OBJCOPY) -v -O binary $(ELF_FILE)  $(BIN_FILE)
	@python3 $(TL_CHECK) $(BIN_FILE)
	@echo 'Finished building: $@'
	@echo ' '
	@cp $(BIN_FILE) $(PROJECT_NAME)_$(VERSION_RELEASE).$(VERSION_BUILD).bin
	@echo 'Create zigbee OTA file'
	@python3 $(MAKE_OTA) -ot $(PROJECT_NAME) $(PROJECT_NAME)_$(VERSION_RELEASE).$(VERSION_BUILD).bin
	@echo ' '
#	@echo GCC_FLAGS: $(GCC_FLAGS)
	 

sizedummy: $(ELF_FILE)
	@echo 'Invoking: Print Size'
	@$(SIZE) -t $(ELF_FILE)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE) $(ELFS) $(OBJS) $(SIZEDUMMY) $(LST_FILE) $(ELF_FILE) $(PROJECT_NAME)_$(VERSION_RELEASE).$(VERSION_BUILD).bin *.zigbee
	-@echo ' '

clean-project:
	-$(RM) $(FLASH_IMAGE) $(ELFS) $(SIZEDUMMY) $(LST_FILE) $(ELF_FILE) $(PROJECT_NAME)_$(VERSION_RELEASE).$(VERSION_BUILD).bin *.zigbee
	-$(RM) -R $(OUT_PATH)/$(SRC_PATH)/*.o
	-@echo ' '
	
pre-build:
	mkdir -p $(foreach s,$(OUT_DIR),$(OUT_PATH)$(s))
#	-" $(SDK_PATH)/tools/tl_link_load.sh" " $(SDK_PATH)/platform/boot/8258/boot_8258.link" "C:\TelinkSDK\SDK\tl_zigbee_sdk\build\tlsr_tc32/boot.link"
	-@echo ' '

post-build:
	-"$(TOOLS_PATH)/tl_check_fw.sh" $(OUT_PATH)/$(PROJECT_NAME) tc32
	-@echo ' '
	
secondary-outputs: $(BIN_FILE) $(LST_FILE) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents pre-build
.SECONDARY: main-build pre-build post-build

