GCC=gcc-4.8
CFLAGS=-Wall -g

run:
	make fat32 && ./fat32 /dev/usbstick

fat32: main.o command_handlers.o program_logic.o
	$(GCC) $(CFLAGS) main.o command_handlers.o program_logic.o -o fat32 

main.o: main.c common.h
	$(GCC) $(CFLAGS) -c main.c

command_handlers.o: command_handlers.c command_handlers.h common.h
	$(GCC) $(CFLAGS) -c command_handlers.c

program_logic.o: program_logic.c program_logic.h common.h
	$(GCC) $(CFLAGS) -c program_logic.c