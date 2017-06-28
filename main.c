#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include "fat32.h"

#define MAX_BUF 1024

//possible user entries into terminal
const char * INFO_COMMAND = "info";
const char * DIR_COMMAND = "dir";
const char * CD_COMMAND = "cd";
const char * GET_COMMAND = "get";
const char * INVALID_COMMAND = "Invalid command!\n";
//string representatiions of the code
#define QUIT_CODE 0
#define INFO_CODE 1
#define DIR_CODE 2
#define CD_CODE 3
#define GET_CODE 4
#define INVALID_CODE -1

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

void print_info_header(char *str) {
    write(STDOUT_FILENO, "----", 4);
    write(STDOUT_FILENO, str, strlen(str));
    write(STDOUT_FILENO, "----\n", 5);
}

void print_info(char *title, char *info, int length) {
    write(STDOUT_FILENO, title, strlen(title));
    write(STDOUT_FILENO, " :", 2);
    write(STDOUT_FILENO, info, length);
    write(STDOUT_FILENO, "\n", 1);
}

void print_device_info(fat32BS *bs) {
    //Device Info
    print_info_header("Device Info");
    print_info("OEM Name", bs->BS_OEMName, BS_OEMName_LENGTH);
    print_info("Label", bs->BS_VolLab, BS_VolLab_LENGTH);
    char mediaType[20];
    sprintf(mediaType, "%#02x (%s) ", bs->BPB_Media, ( bs->BPB_Media == 0xf8 ? "fixed" : "removable") );
    print_info("Media Type", mediaType, strlen(mediaType));
    print_info("Size", mediaType, strlen(mediaType));
    print_info("Drive Number", mediaType, strlen(mediaType));

/**
     OEM Name: %s\n\
     Label: %s\n\
     File System Type: %s\n\
     Media Type: %#02x (%s)\n";
     char print_buf[MAX_BUF];
     sprintf(print_buf, deviceInfo, bs->BS_OEMName, bs->BS_VolLab, bs->BS_FilSysType, bs->BPB_Media, mediaType);
     write(STDOUT_FILENO, print_buf, strlen(print_buf));
     **/
     write(STDOUT_FILENO, "\n", 1);
}

int open_device(char *drive_location) {
    int fd = open(drive_location, O_RDONLY);
    if (fd == -1) {
        perror("open_device");
        exit(EXIT_FAILURE);
    }
    return fd;
}

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
            switch(user_command) {
                case INVALID_CODE: {
                    write(STDOUT_FILENO, INVALID_COMMAND, strlen(INVALID_COMMAND));
                    break;
                }
                case INFO_CODE: {
                    print_device_info(bs);
                    break;
                }
                default: {
                    write(STDOUT_FILENO, "Valid command!\n", 15);
                    break;
                }
            }
        }

    }
    
    return 0;
}