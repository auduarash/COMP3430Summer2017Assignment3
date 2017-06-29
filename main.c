#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "fat32.h"
#include "command_handlers.h"
#include "program_logic.h"

//possible user entries into terminal
char*INFO_COMMAND = "info";
char*DIR_COMMAND = "dir";
char*CD_COMMAND = "cd";
char*GET_COMMAND = "get";
char*INVALID_COMMAND = "Invalid command!\n";

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

    printf("Drive location is %s.\n", argv[1]);


    int fd = open_device(argv[1]);
    fat32BS *bs = load_bpb_params(fd);

    while (true) {
        int user_command = get_next_command();
        if (user_command == 0) {
            break;
        } else {
            handle_user_choice(bs, user_command);
        }

    }
    
    return 0;
}