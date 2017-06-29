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

char printBuf[MAX_BUF];
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


void print_info(char *info, int length) {
    int i;
    for (i = 0; i < length; i++) {
        printBuf[i] = info[i];
    }
    printBuf[length] = '\0';
}

void print_device_info(fat32BS *bs) {
    //Device Info
    printf("---Device Info---\n");
    print_info(bs->BS_OEMName, BS_OEMName_LENGTH);
    printf("OEM Name: %s\n", printBuf);
    print_info(bs->BS_VolLab, BS_VolLab_LENGTH);
    printf("Label: %s\n", printBuf);
    printf("Media Type: %#02x (%s)\n", bs->BPB_Media, ( bs->BPB_Media == 0xf8 ? "fixed" : "removable") );
    // long size = bs->BPB_SecPerClus; 
    long size = bs->BPB_BytesPerSec;
    size *= bs->BPB_TotSec32;
    long sizeMB = size / ( 1024 * 1024);
    double sizeGB = sizeMB / 1024.0;
    printf("Size: %lu bytes (%luMB, %.3fGB)\n", size, sizeMB, sizeGB);
    printf("Drive Number: %d (%s)\n", bs->BS_DrvNum, (bs->BS_DrvNum == 0x00 ? "floppy" : "hard disk"));

    printf("\n---Geometry---\n");
    printf("Bytes per sector: %d\n", bs->BPB_BytesPerSec);
    printf("Sectors per cluster: %d\n", bs->BPB_SecPerClus);
    printf("Total sectors: %d\n", bs->BPB_TotSec32);

    printf("\n---FS Info---\n");
    print_info(bs->BS_VolLab, BS_VolLab_LENGTH);
    printf("Volume ID: %s\n", printBuf);
    printf("Version: %d:%d\n", bs->BPB_FSVerLow, bs->BPB_FSVerHigh);
    printf("Reserved Sectors: %d\n", bs->BPB_RsvdSecCnt);
    printf("Number of FATs: %d\n", bs->BPB_NumFATs);
    printf("FAT Size: %d\n", bs->BPB_FATSz32);
    int mirrored = ( 7 & bs->BPB_ExtFlags); //last 3 bits tell number of mirrored fats
    int mirrored_enabled = (128 & bs->BPB_ExtFlags);
    if (mirrored_enabled == 0) mirrored = 0;
    printf("Mirrored FAT: %d (%s)\n", mirrored, (mirrored_enabled ? "yes" : "no"));
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