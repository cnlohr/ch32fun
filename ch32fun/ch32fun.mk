ifeq ($(OS),Windows_NT)
	WHICH:=where
	NULLDEV:=nul
else
	WHICH:=which
	NULLDEV:=/dev/null
endif

# Default/fallback prefix
PREFIX_DEFAULT:=riscv64-elf

ifneq ($(shell $(WHICH) riscv64-unknown-elf-gcc 2>$(NULLDEV)),)
	PREFIX_DEFAULT:=riscv64-unknown-elf
else ifneq ($(shell $(WHICH) riscv-none-elf-gcc 2>$(NULLDEV)),)
	PREFIX_DEFAULT:=riscv-none-elf
else ifneq ($(shell $(WHICH) riscv64-unknown-none-elf-gcc 2>$(NULLDEV)),)
	PREFIX_DEFAULT:=riscv64-unknown-none-elf
endif

PREFIX?=$(PREFIX_DEFAULT)
# We used to check if riscv64-linux-gnu-gcc exists, because it would still produce valid output with -ffreestanding.
# It was different enough that we decided not to automatically fallback to it.

# Fedora places newlib in a different location
ifneq ($(wildcard /etc/fedora-release),)
	NEWLIB?=/usr/arm-none-eabi/include
else
	NEWLIB?=/usr/include/newlib
endif

CH32FUN?=$(dir $(lastword $(MAKEFILE_LIST)))
#TARGET_MCU?=CH32V003 # Because we are now opening up to more processors, don't assume this.

TARGET_EXT?=c

CH32FUN?=$(dir $(lastword $(MAKEFILE_LIST)))
MINICHLINK?=$(CH32FUN)/../minichlink

WRITE_SECTION?=flash
SYSTEM_C?=$(CH32FUN)/ch32fun.c

ifeq ($(DEBUG),1)
	EXTRA_CFLAGS+=-DFUNCONF_DEBUG=1
endif

CFLAGS?=-g -Os -flto -ffunction-sections -fdata-sections -fmessage-length=0 -msmall-data-limit=8
LDFLAGS+=-Wl,--print-memory-usage -Wl,-Map=$(TARGET).map

# Get GCC major version in a shell-agnostic way
GCCVERSION := $(shell $(PREFIX)-gcc -dumpversion)
GCCMAJOR := $(firstword $(subst ., ,$(GCCVERSION)))
# every major version below 13 maps to 0, anything 13 or above maps to 1
GCCVERSION13 := $(if $(filter 1 2 3 4 5 6 7 8 9 10 11 12,$(GCCMAJOR)),0,1)

TARGET_MCU_PACKAGE?=$(TARGET_MCU)
TARGET_MCU_MEMORY_SPLIT?=3
ENABLE_FPU?=1
	
_:= $(shell sh $(CH32FUN)/parse_mcu_package.sh $(TARGET_MCU_PACKAGE) $(TARGET_MCU_MEMORY_SPLIT) $(ENABLE_FPU) > $(CH32FUN)/generated_config.mk)
ifneq ($(.SHELLSTATUS),0)
$(error "Configuration script failed with exit code $(.SHELLSTATUS). Please check the error message above.")
endif
include $(CH32FUN)/generated_config.mk

CFLAGS_CONFIG:=-D$(TARGET_MCU)=1 -DFLASH_SIZE_KB=$(FLASH_SIZE_KB) -DRAM_SIZE_KB=$(RAM_SIZE_KB)
ifneq ($(EXT_ORIGIN),)
CFLAGS_CONFIG+=-DEXT_ORIGIN=$(EXT_ORIGIN) -DEXT_SIZE_KB=$(EXT_SIZE_KB)
endif
ifneq ($(TARGET_MCU_MEMORY_SPLIT),)
CFLAGS_CONFIG+=-DTARGET_MCU_MEMORY_SPLIT=$(TARGET_MCU_MEMORY_SPLIT)
endif
CFLAGS_CONFIG+=$(foreach define,$(DEFINES),-D$(define)=1)

ifeq ($(ARCH),rv32ec_zmmul)
ifneq ($(GCCVERSION13),1)
$(warning "GCC version 13 or higher is required for rv32ec_zmmul, but detected version $(GCCVERSION). Falling back to rv32ec.")
ARCH=rv32ec
endif
endif

CFLAGS_ARCH+=-march=$(ARCH) -mabi=$(ABI)

IS_CH5XX:=$(if $(filter CH5%, $(TARGET_MCU)),1,0)
ifeq ($(IS_CH5XX),1)
5xx_enable_swd :
	echo "Press and hold the download button when applying power and plug into the USB port before executing."
	$(MINICHLINK)/minichlink -N
5xx_unbrick :
	echo "Press and hold the download button when applying power and plug into the USB port before executing."
	$(MINICHLINK)/minichlink -E
endif

ifneq ($(filter CH32V00%, $(TARGET_MCU)),)
	# CH32V00x needs a special version of libgcc.a
	LDFLAGS+=-L$(CH32FUN)/../misc -lgcc
else
	LDFLAGS+=-lgcc
endif
GENERATED_LD_FILE:=$(CH32FUN)/generated_$(TARGET_MCU_PACKAGE)_$(TARGET_MCU_MEMORY_SPLIT).ld
LINKER_SCRIPT?=$(GENERATED_LD_FILE)

CFLAGS+= \
	$(CFLAGS_ARCH) -static-libgcc \
	$(CFLAGS_CONFIG) \
	-I$(NEWLIB) \
	-I$(CH32FUN)/../extralibs \
	-I$(CH32FUN) \
	-nostdlib \
	-I. -Wall $(EXTRA_CFLAGS)

LDFLAGS+=-T $(LINKER_SCRIPT) -Wl,--gc-sections
FILES_TO_COMPILE:=$(SYSTEM_C) $(TARGET).$(TARGET_EXT) $(ADDITIONAL_C_FILES) 

$(TARGET).bin : $(TARGET).elf
	$(PREFIX)-objdump -S $^ > $(TARGET).lst
	$(PREFIX)-objcopy -R .storage  -O binary $< $(TARGET).bin
	$(PREFIX)-objcopy -j .storage -O binary $< $(TARGET)_ext.bin
	$(PREFIX)-objcopy -O ihex $< $(TARGET).hex

ifeq ($(OS),Windows_NT)
closechlink :
	-taskkill /F /IM minichlink.exe /T
else
closechlink :
	-killall minichlink
endif

terminal : monitor

monitor :
	$(MINICHLINK)/minichlink -T

unbrick :
	$(MINICHLINK)/minichlink -u

gdbserver : 
	-$(MINICHLINK)/minichlink -baG

gdbclient :
	gdb-multiarch $(TARGET).elf -ex "target remote :3333"

clangd :
	make clean
	bear -- make build

clangd_clean :
	rm -f compile_commands.json
	rm -rf .cache

FLASH_COMMAND?=$(MINICHLINK)/minichlink -w $< $(WRITE_SECTION) -b
FLASH_EXT_COMMAND?=$(MINICHLINK)/minichlink -w $< $(EXT_ORIGIN) -b

.PHONY : $(GENERATED_LD_FILE)
$(GENERATED_LD_FILE) :
	$(PREFIX)-gcc -E -P -x c $(CFLAGS_CONFIG) $(CH32FUN)/ch32fun.ld > $(GENERATED_LD_FILE)

$(TARGET).elf : $(FILES_TO_COMPILE) $(LINKER_SCRIPT) $(EXTRA_ELF_DEPENDENCIES)
	$(PREFIX)-gcc -o $@ $(FILES_TO_COMPILE) $(CFLAGS) $(LDFLAGS)

# Rule for independently building ch32fun.o indirectly, instead of recompiling it from source every time.
# Not used in the default 003fun toolchain, but used in more sophisticated toolchains.
ch32fun.o : $(SYSTEM_C)
	$(PREFIX)-gcc -c -o $@ $(SYSTEM_C) $(CFLAGS)

# Only rebuild minichlink if it doesn't exist at all.
$(MINICHLINK)/minichlink :
	make -C $(MINICHLINK) all

cv_flash : $(TARGET).bin $(MINICHLINK)/minichlink
	$(FLASH_COMMAND)

cv_flash_ext : $(TARGET)_ext.bin
	make -C $(MINICHLINK) all
	$(FLASH_EXT_COMMAND)

cv_clean :
	rm -rf $(TARGET).elf $(TARGET).bin $(TARGET)_ext.bin $(TARGET).hex $(TARGET).lst $(TARGET).map $(TARGET).hex $(GENERATED_LD_FILE) || true

build : $(TARGET).bin
