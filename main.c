#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "fat32.h"
#include "fat32_impl.h"
#include "command_handlers.h"
#include "program_logic.h"




fat32BS *bs = NULL;
fat32DE *curr_dir = NULL;
int fd = -1;

void load_bpb_params(int fd) {
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


    open_device(argv[1]);
    load_bpb_params(fd);

    set_root_dir(fd);

    while (true) {
        int user_command = get_next_command();
        if (user_command == 0) {
            break;
        } else {
            handle_user_choice(user_command);
        }

    }
    
    return 0;
}