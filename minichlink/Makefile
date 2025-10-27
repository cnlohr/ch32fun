TOOLS:=minichlink minichlink.so

CFLAGS:=-O0 -g3 -Wall -Wno-unused-function -DCH32V003 -I. -DMINICHLINK
C_S:=minichlink.c pgm-wch-linke.c pgm-wch-isp.c pgm-esp32s2-ch32xx.c nhc-link042.c ardulink.c serial_dev.c pgm-b003fun.c minichgdb.c chips.c ch5xx.c
H_S:=cmdserver.h funconfig.h hidapi.h libusb.h microgdbstub.h minichlink.h serial_dev.h terminalhelp.h

# General Note: To use with GDB, gdb-multiarch
# gdb-multilib {file}
# target remote :2345

CFLAGS_WINDOWS:=-Os -s -Wall -D_WIN32_WINNT=0x0600 -I. -Wall -DCH32V003 -DMINICHLINK
LDFLAGS_WINDOWS:=-L. -lpthread -lusb-1.0 -lsetupapi -lws2_32
ifeq ($(OS),Windows_NT)
	TOOLS:=minichlink.exe
else
	OS_NAME := $(shell uname -s | tr A-Z a-z)
	ifeq ($(OS_NAME),linux)
		LDFLAGS:=-lpthread -lusb-1.0 -ludev
	endif
	ifeq ($(OS_NAME),darwin)
		# Use ARCH flag (e.g. x86_64 or arm64) if given externally, otherwise empty
		ifdef ARCH
			ARCHFLAG := -arch $(ARCH)
		else
			ARCHFLAG :=
		endif
		# Use custom libusb includes/libs if provided, otherwise pkg-config will find it
		ifeq ($(strip $(LIBUSB_INCS)),)
			LIBUSB_INCS := $(shell pkg-config --cflags libusb-1.0)
		endif
		ifeq ($(strip $(LIBUSB_LIBS)),)
			LIBUSB_LIBS := $(shell pkg-config --libs libusb-1.0)
		endif

		LDFLAGS := -lpthread -framework CoreFoundation -framework IOKit -framework Security $(LIBUSB_LIBS)
		CFLAGS := $(ARCHFLAG) -O0 -Wall -Wno-asm-operand-widths -Wno-deprecated-declarations -Wno-deprecated-non-prototype -D__MACOSX__ -DMINICHLINK -DCH32V003 -I. $(LIBUSB_INCS)
	endif
endif

all : $(TOOLS)

# will need mingw-w64-x86-64-dev gcc-mingw-w64-x86-64
minichlink.exe : $(C_S) $(H_S) Makefile
	x86_64-w64-mingw32-gcc -o $@ $(C_S) $(LDFLAGS_WINDOWS) $(CFLAGS_WINDOWS)

minichlink : $(C_S) $(H_S) Makefile
	gcc -o $@ $(C_S) $(LDFLAGS) $(CFLAGS) $(INCS)

minichlink.so : $(C_S) $(H_S) Makefile
	gcc -o $@ $(C_S) $(LDFLAGS) $(CFLAGS) $(INCS) -shared -fPIC

minichlink.dll : $(C_S) $(H_S) Makefile
	x86_64-w64-mingw32-gcc -o $@ $(C_S) $(LDFLAGS_WINDOWS) $(CFLAGS_WINDOWS) $(INCS) -shared -DMINICHLINK_AS_LIBRARY

install_udev_rules :
	cp 99-minichlink.rules /etc/udev/rules.d/
	udevadm control --reload
	udevadm trigger

inspect_bootloader : minichlink
	./minichlink -r test.bin launcher 0x780
	riscv64-unknown-elf-objdump -S -D test.bin -b binary -m riscv:rv32 | less

clean :
	rm -rf $(TOOLS) minichlink.exe
