#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "fat32.h"
#include "program_logic.h"
#include "command_handlers.h"

int get_next_command() {
    static char *input_signal = "> ";
    static int input_size;
    static char buf[MAX_BUF];

    write(STDOUT_FILENO, input_signal, 2);
    input_size = read(STDIN_FILENO, buf, MAX_BUF);
    int return_code = INVALID_CODE; //User wishes to exit the program
    if (input_size < 0) {

        perror("read");
        exit(EXIT_FAILURE);

    } else if (input_size == 0) {

        return_code = QUIT_CODE;

    } else {
        buf[input_size-1] = '\0'; //terminate the string properly
        if (strcmp(INFO_COMMAND, buf) == 0) return_code = INFO_CODE;
        else if (strcmp(DIR_COMMAND, buf) == 0) return_code = DIR_CODE;
        else if (strcmp(CD_COMMAND, buf) == 0) return_code = CD_CODE;
        else if (strcmp(GET_COMMAND, buf) == 0) return_code = GET_CODE;

    }
    return return_code;

}


void handle_user_choice(fat32BS *bs, int user_choice) {

    switch(user_choice) {
        case INVALID_CODE: {
            write(STDOUT_FILENO, INVALID_COMMAND, strlen(INVALID_COMMAND));
            break;
        }
        case INFO_CODE: {
            print_device_info(bs);
            break;
        }
        case DIR_CODE: {

        }
        default: {
            write(STDOUT_FILENO, "Valid command!\n", 15);
            break;
        }
    }
}



int open_device(char *drive_location) {
    int fd = open(drive_location, O_RDONLY);
    if (fd == -1) {
        perror("open_device");
        exit(EXIT_FAILURE);
    }
    return fd;
}