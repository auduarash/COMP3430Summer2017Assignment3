#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_BUF 1024
/**
    main.c
    
    Implementation of a fat32 file reader.

    @author Abdul-Rasheed Audu
    @since 24/06/2017
*/

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s {usbdevice location}\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *drive_location = argv[1];
    printf("Drive location is %s.\n", drive_location);
    char buf[MAX_BUF];
    char *input_signal = "> ";
    int input_size;
    while (true) {
        // printf("> ");
        write(STDOUT_FILENO, input_signal, 3);

        input_size = read(STDIN_FILENO, buf, MAX_BUF);
        if (input_size == 0) break; //CTRL + D was pressed
        write(STDOUT_FILENO, buf, input_size);

    }
    
    return 0;
}