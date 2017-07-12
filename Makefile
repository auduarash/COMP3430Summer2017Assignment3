GCC=gcc-4.8
CFLAGS=-Wall -g

default: fat32

run:
	make fat32 && ./fat32 /dev/usbstick

fat32: fat32_calculations.o utilities.o common.o main.o program_logic.o fat32_impl.o
	$(GCC) $(CFLAGS) fat32_calculations.o utilities.o common.o main.o program_logic.o fat32_impl.o -o fat32 

main.o: main.c common.h
	$(GCC) $(CFLAGS) -c main.c

program_logic.o: program_logic.c program_logic.h common.h
	$(GCC) $(CFLAGS) -c program_logic.c

fat32_impl.p: fat32_impl.c fat32_impl.h common.h
	$(GCC) $(CFLAGS) -c fat32_impl.c

common.o: common.c common.h
	$(GCC) $(CFLAGS) -c common.c

fat32_calculations.o: fat32_calculations.c fat32_calculations.h
	$(GCC) $(CFLAGS) -c fat32_calculations.c

utilities.o: utilities.c utilities.h
	$(GCC) $(CFLAGS) -c utilities.c

clean:
	rm -rf *.o && rm -rf fat32