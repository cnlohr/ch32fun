all : usbhidtool

# to make CI happy.
usbhidtool.hidapi.elf : usbhidtool.c
	gcc -o $@ $^ -I../../../../minichlink -ludev

usbhidtool : usbhidtool.c
	gcc -o $@ $^ -I../../../../minichlink -ludev

go_into_bootloader : usbhidtool
	./usbhidtool -r

usermonitor : usbhidtool
	./usbhidtool -t

clean :
	rm -rf *.o *~ usbhidtool

