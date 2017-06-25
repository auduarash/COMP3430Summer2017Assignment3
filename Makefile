GCC=gcc-4.8
CFLAGS=-Wall -g

run:
	make fat32 && ./fat32 /dev/usbstick

fat32: main.o
	$(GCC) $(CFLAGS) main.o -o fat32 

main.o: main.c
	$(GCC) $(CFLAGS) -c main.c