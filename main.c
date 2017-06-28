#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include "fat32.h"

#define MAX_BUF 1024
fat32BS * load_bpb_params(int fd) {
    fat32BS *bs;
    int array_size = sizeof(fat32BS) / sizeof(char);
    char bs_bpb[ array_size  ];

    int seek = lseek(fd, 0, SEEK_SET);
    if (seek < 0) {
        perror("seek");
        exit(EXIT_FAILURE);
    }
    int read_size = read(fd, bs_bpb, array_size);
    printf("Read %d characters\n", read_size);
    if (read_size < 0) {
        perror("read bpb");
        exit(EXIT_FAILURE);
    }
    bs = malloc(sizeof(fat32BS));
    memcpy(bs, bs_bpb, sizeof(fat32BS));
    return bs;
}


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
    int fd = open(drive_location, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    fat32BS *bs = load_bpb_params(fd);
    write(STDOUT_FILENO, bs->BS_OEMName, strlen(bs->BS_OEMName));

    while (true) {
        // printf("> ");
        write(STDOUT_FILENO, input_signal, 3);

        input_size = read(STDIN_FILENO, buf, MAX_BUF);
        if (input_size == 0) break; //CTRL + D was pressed
        write(STDOUT_FILENO, buf, input_size);

    }
    
    return 0;
}