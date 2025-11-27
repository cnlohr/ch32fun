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

ifeq ($(findstring CH32V00,$(TARGET_MCU)),CH32V00) # CH32V002, 3, 4, 5, 6, 7
	MCU_PACKAGE?=1
	GENERATED_LD_FILE?=$(CH32FUN)/generated_ch32v003.ld
	LINKER_SCRIPT?=$(GENERATED_LD_FILE)
	LDFLAGS+=-L$(CH32FUN)/../misc -lgcc
	# Note: The CH32V003 is different from the other 00x variants and needs a different ARCH
	ifeq ($(TARGET_MCU),CH32V003)
		CFLAGS_ARCH+=-march=rv32ec -mabi=ilp32e
	else ifeq "$(GCCVERSION13)" "1"
		CFLAGS_ARCH+=-march=rv32ec_zmmul -mabi=ilp32e -DCH32V00x=1
	else
		CFLAGS_ARCH+=-march=rv32ec -mabi=ilp32e -DCH32V00x=1  # If not GCC 13 or higher, does not support zmmul as a command line
	endif

	CFLAGS_ARCH+=-D$(TARGET_MCU)

	ifeq ($(findstring CH32V003, $(TARGET_MCU)), CH32V003)
		TARGET_MCU_LD:=0
	else ifeq ($(findstring CH32V002, $(TARGET_MCU)), CH32V002)
		TARGET_MCU_LD:=5
	else ifeq ($(findstring CH32V004, $(TARGET_MCU)), CH32V004)
		TARGET_MCU_LD:=6
	else ifeq ($(findstring CH32V005, $(TARGET_MCU)), CH32V005)
		TARGET_MCU_LD:=6
	else ifeq ($(findstring CH32V006, $(TARGET_MCU)), CH32V006)
		TARGET_MCU_LD:=7
	else ifeq ($(findstring CH32V007, $(TARGET_MCU)), CH32V007)
		TARGET_MCU_LD:=7
	else
		ERROR:=$(error Unknown MCU $(TARGET_MCU))
	endif
else ifeq ($(findstring CH32V10,$(TARGET_MCU)),CH32V10) # CH32V103
	TARGET_MCU_PACKAGE?=CH32V103R8T6
	CFLAGS_ARCH+=	-march=rv32imac \
		-mabi=ilp32 \
		-DCH32V10x=1

	# MCU Flash/RAM split
	ifeq ($(findstring R8, $(TARGET_MCU_PACKAGE)), R8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring C8, $(TARGET_MCU_PACKAGE)), C8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring C6, $(TARGET_MCU_PACKAGE)), C6)
		MCU_PACKAGE:=2
	endif

	TARGET_MCU_LD:=1
else ifeq ($(findstring CH32X03,$(TARGET_MCU)),CH32X03) # CH32X033, X035
	TARGET_MCU_PACKAGE?=CH32X035F8U6
	CFLAGS_ARCH+=-march=rv32imac \
		-mabi=ilp32 \
		-DCH32X03x=1

	# MCU Flash/RAM split
	ifeq ($(findstring F8, $(TARGET_MCU_PACKAGE)), F8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring R8, $(TARGET_MCU_PACKAGE)), R8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring K8, $(TARGET_MCU_PACKAGE)), K8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring C8, $(TARGET_MCU_PACKAGE)), C8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring G8, $(TARGET_MCU_PACKAGE)), G8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring G6, $(TARGET_MCU_PACKAGE)), G6)
		MCU_PACKAGE:=1
	else ifeq ($(findstring F7, $(TARGET_MCU_PACKAGE)), F7)
		MCU_PACKAGE:=1
	endif

	TARGET_MCU_LD:=4		
else ifeq ($(findstring CH32L103,$(TARGET_MCU)),CH32L103) # CH32L103
	TARGET_MCU_PACKAGE?=CH32L103C8T6
	CFLAGS_ARCH+=-march=rv32imac \
		-mabi=ilp32 \
		-DCH32L103=1

	# MCU Flash/RAM split
	ifeq ($(findstring F8, $(TARGET_MCU_PACKAGE)), F8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring K8, $(TARGET_MCU_PACKAGE)), K8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring C8, $(TARGET_MCU_PACKAGE)), C8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring G8, $(TARGET_MCU_PACKAGE)), G8)
		MCU_PACKAGE:=1
	endif

	TARGET_MCU_LD:=4
else ifeq ($(findstring CH32V20,$(TARGET_MCU)),CH32V20) # CH32V203
	TARGET_MCU_PACKAGE?=CH32V203F6P6
	CFLAGS_ARCH+=	-march=rv32imac \
		-mabi=ilp32 \
		-DCH32V20x=1

	# MCU Flash/RAM split

	# Package
	ifeq ($(findstring 203RB, $(TARGET_MCU_PACKAGE)), 203RB)
		CFLAGS+=-DCH32V20x_D8
	else ifeq ($(findstring 208, $(TARGET_MCU_PACKAGE)), 208)
		CFLAGS+=-DCH32V20x_D8W
		MCU_PACKAGE:=3
	else ifeq ($(findstring F8, $(TARGET_MCU_PACKAGE)), F8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring G8, $(TARGET_MCU_PACKAGE)), G8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring K8, $(TARGET_MCU_PACKAGE)), K8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring C8, $(TARGET_MCU_PACKAGE)), C8)
		MCU_PACKAGE:=1
	else ifeq ($(findstring F6, $(TARGET_MCU_PACKAGE)), F6)
		MCU_PACKAGE:=2
	else ifeq ($(findstring G6, $(TARGET_MCU_PACKAGE)), G6)
		MCU_PACKAGE:=2
	else ifeq ($(findstring K6, $(TARGET_MCU_PACKAGE)), K6)
		MCU_PACKAGE:=2
	else ifeq ($(findstring C6, $(TARGET_MCU_PACKAGE)), C6)
		MCU_PACKAGE:=2
	else ifeq ($(findstring RB, $(TARGET_MCU_PACKAGE)), RB)
		MCU_PACKAGE:=3
	else ifeq ($(findstring GB, $(TARGET_MCU_PACKAGE)), GB)
		MCU_PACKAGE:=3
	else ifeq ($(findstring CB, $(TARGET_MCU_PACKAGE)), CB)
		MCU_PACKAGE:=3
	else ifeq ($(findstring WB, $(TARGET_MCU_PACKAGE)), WB)
		MCU_PACKAGE:=3
	else
		CFLAGS+=-DCH32V20x_D6
	endif

	# MCU EXT Flash
	ifeq ($MCU_PACKAGE,1)
		EXT_ORIGIN:=0x08010000
	else ifeq ($MCU_PACKAGE,2)
		EXT_ORIGIN:=0x08008000
	else ifeq ($MCU_PACKAGE,3)
		EXT_ORIGIN:=0x08020000
	endif

	TARGET_MCU_LD:=2
else ifeq ($(findstring CH32V30,$(TARGET_MCU)),CH32V30) #CH32V307
	TARGET_MCU_PACKAGE?=CH32V307VCT6
	MCU_PACKAGE?=1
	TARGET_MCU_MEMORY_SPLIT?=3
	ENABLE_FPU?=1

	ifeq ($(ENABLE_FPU), 1)
		CFLAGS_ARCH+= -march=rv32imafc -mabi=ilp32f
	else
		CFLAGS_ARCH+= -march=rv32imac -mabi=ilp32 -DDISABLED_FLOAT
	endif

	CFLAGS_ARCH+= \
		-DCH32V30x=1 \
		-DTARGET_MCU_MEMORY_SPLIT=$(TARGET_MCU_MEMORY_SPLIT)

	# MCU Flash/RAM split
	ifeq ($(findstring RC, $(TARGET_MCU_PACKAGE)), RC)
		MCU_PACKAGE:=1
	else ifeq ($(findstring VC, $(TARGET_MCU_PACKAGE)), VC)
		MCU_PACKAGE:=1
	else ifeq ($(findstring WC, $(TARGET_MCU_PACKAGE)), WC)
		MCU_PACKAGE:=1
	else ifeq ($(findstring CB, $(TARGET_MCU_PACKAGE)), CB)
		MCU_PACKAGE:=2
	else ifeq ($(findstring FB, $(TARGET_MCU_PACKAGE)), FB)
		MCU_PACKAGE:=2
	else ifeq ($(findstring RB, $(TARGET_MCU_PACKAGE)), RB)
		MCU_PACKAGE:=2
	endif

	# Package
	ifeq ($(findstring 303, $(TARGET_MCU_PACKAGE)), 303)
		CFLAGS+=-DCH32V30x_D8
	else
		CFLAGS+=-DCH32V30x_D8C
	endif

	TARGET_MCU_LD:=3
else ifeq ($(findstring CH57,$(TARGET_MCU)),CH57) # CH570 1 2 3
	TARGET_MCU_PACKAGE?=CH570E
	CFLAGS_ARCH+=-march=rv32imac \
		-mabi=ilp32 \
		-DCH57x=1 \
		-DCH5xx

	# MCU Flash/RAM split
	ifeq ($(findstring 570, $(TARGET_MCU_PACKAGE)), 570)
		MCU_PACKAGE:=0
	else ifeq ($(findstring 571, $(TARGET_MCU_PACKAGE)), 571)
		MCU_PACKAGE:=1
	else ifeq ($(findstring 572, $(TARGET_MCU_PACKAGE)), 572)
		MCU_PACKAGE:=2
	else ifeq ($(findstring 573, $(TARGET_MCU_PACKAGE)), 573)
		MCU_PACKAGE:=3
	endif
	CFLAGS+=-DMCU_PACKAGE=$(MCU_PACKAGE)

	# Package
	ifeq ($(findstring D, $(TARGET_MCU_PACKAGE)), D)
		CFLAGS+=-DCH57xD
	else ifeq ($(findstring Q, $(TARGET_MCU_PACKAGE)), Q)
		CFLAGS+=-DCH57xQ
	else ifeq ($(findstring R, $(TARGET_MCU_PACKAGE)), R)
		CFLAGS+=-DCH57xR
	else ifeq ($(findstring E, $(TARGET_MCU_PACKAGE)), E)
		CFLAGS+=-DCH57xE
	endif

	TARGET_MCU_LD:=10
	IS_CH5XX:=1
else ifeq ($(findstring CH58,$(TARGET_MCU)),CH58) # CH582/3/4/5
	TARGET_MCU_PACKAGE?=CH582F
	CFLAGS_ARCH+=-march=rv32imac \
		-mabi=ilp32 \
		-DCH58x=1 \
		-DCH5xx

	# MCU Flash/RAM split
	ifeq ($(findstring 582, $(TARGET_MCU_PACKAGE)), 582)
		MCU_PACKAGE:=2
	else ifeq ($(findstring 583, $(TARGET_MCU_PACKAGE)), 583)
		MCU_PACKAGE:=3
	else ifeq ($(findstring 584, $(TARGET_MCU_PACKAGE)), 584)
		MCU_PACKAGE:=4
	else ifeq ($(findstring 585, $(TARGET_MCU_PACKAGE)), 585)
		MCU_PACKAGE:=5
	endif
	CFLAGS+=-DMCU_PACKAGE=$(MCU_PACKAGE)

	# Package
	ifeq ($(findstring D, $(TARGET_MCU_PACKAGE)), D)
		CFLAGS+=-DCH58xD
	else ifeq ($(findstring F, $(TARGET_MCU_PACKAGE)), F)
		CFLAGS+=-DCH58xF
	else ifeq ($(findstring M, $(TARGET_MCU_PACKAGE)), M)
		CFLAGS+=-DCH58xM
	endif

	TARGET_MCU_LD:=8
	IS_CH5XX:=1
else ifeq ($(findstring CH59,$(TARGET_MCU)),CH59) # CH592 1
	TARGET_MCU_PACKAGE?=CH592F
	CFLAGS_ARCH+=-march=rv32imac \
		-mabi=ilp32 \
		-DCH59x=1 \
		-DCH5xx

	# MCU Flash/RAM split
	ifeq ($(findstring 591, $(TARGET_MCU_PACKAGE)), 591)
		MCU_PACKAGE:=1
	else ifeq ($(findstring 592, $(TARGET_MCU_PACKAGE)), 592)
		MCU_PACKAGE:=2
	endif
	CFLAGS+=-DMCU_PACKAGE=$(MCU_PACKAGE)

	# Package
	ifeq ($(findstring D, $(TARGET_MCU_PACKAGE)), D)
		CFLAGS+=-DCH59xD
	else ifeq ($(findstring F, $(TARGET_MCU_PACKAGE)), F)
		CFLAGS+=-DCH59xF
	endif

	TARGET_MCU_LD:=9
	IS_CH5XX:=1
else ifeq ($(findstring CH32H41,$(TARGET_MCU)),CH32H41)
	TARGET_MCU_PACKAGE?=CH32H417
	ENABLE_FPU?=1
	MCU_PACKAGE?=1

	ifeq ($(ENABLE_FPU), 1)
		CFLAGS_ARCH+= -march=rv32imafc -mabi=ilp32f
	else
		CFLAGS_ARCH+= -march=rv32imac -mabi=ilp32 -DDISABLED_FLOAT
	endif

	ifeq ($(findstring 416, $(TARGET_MCU_PACKAGE)), 416)
		MCU_PACKAGE:=2
	else ifeq ($(findstring 415, $(TARGET_MCU_PACKAGE)), 415)
		MCU_PACKAGE:=3
	endif

	CFLAGS+=-DMCU_PACKAGE=$(MCU_PACKAGE)
	CFLAGS+=-DCH32H41x
	TARGET_MCU_LD:=11
else
	ERROR:=$(error Unknown MCU $(TARGET_MCU))
endif

ifeq ($(IS_CH5XX),1)
5xx_enable_swd :
	echo "Press and hold the download button when applying power and plug into the USB port before executing."
	$(MINICHLINK)/minichlink -N
5xx_unbrick :
	echo "Press and hold the download button when applying power and plug into the USB port before executing."
	$(MINICHLINK)/minichlink -E
endif


LDFLAGS+=-lgcc
GENERATED_LD_FILE:=$(CH32FUN)/generated_$(TARGET_MCU_PACKAGE)_$(TARGET_MCU_MEMORY_SPLIT).ld
LINKER_SCRIPT?=$(GENERATED_LD_FILE)

CFLAGS+= \
	$(CFLAGS_ARCH) -static-libgcc \
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
	$(PREFIX)-gcc -E -P -x c -DTARGET_MCU=$(TARGET_MCU) -DMCU_PACKAGE=$(MCU_PACKAGE) -DTARGET_MCU_LD=$(TARGET_MCU_LD) -DTARGET_MCU_MEMORY_SPLIT=$(TARGET_MCU_MEMORY_SPLIT) $(CH32FUN)/ch32fun.ld > $(GENERATED_LD_FILE)

$(TARGET).elf : $(FILES_TO_COMPILE) $(LINKER_SCRIPT) $(EXTRA_ELF_DEPENDENCIES)
	$(PREFIX)-gcc -o $@ $(FILES_TO_COMPILE) $(CFLAGS) $(LDFLAGS)

# Rule for independently building ch32fun.o indirectly, instead of recompiling it from source every time.
# Not used in the default 003fun toolchain, but used in more sophisticated toolchains.
ch32fun.o : $(SYSTEM_C)
	$(PREFIX)-gcc -c -o $@ $(SYSTEM_C) $(CFLAGS)

cv_flash : $(TARGET).bin
	make -C $(MINICHLINK) all
	$(FLASH_COMMAND)

cv_flash_ext : $(TARGET)_ext.bin
	make -C $(MINICHLINK) all
	$(FLASH_EXT_COMMAND)

cv_clean :
	rm -rf $(TARGET).elf $(TARGET).bin $(TARGET)_ext.bin $(TARGET).hex $(TARGET).lst $(TARGET).map $(TARGET).hex $(GENERATED_LD_FILE) || true

build : $(TARGET).bin
