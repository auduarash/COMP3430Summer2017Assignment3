#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "common.h"
#include "fat32.h"
#include "fat32_impl.h"
#include "program_logic.h"


/*
    Compares a command to a user enter string.
*/
bool cmdcmp(const char *actual, char user_entry[]) {
    int i = strlen(user_entry) - 1;
    while (i >= 0 && user_entry[i] == ' ') {
        user_entry[i--] = 0;
    }
    char buf[strlen(user_entry)+1];
    int pos;
    int scanned = sscanf(user_entry, "%s%n", buf, &pos);
    
    return scanned == 1 && pos == strlen(user_entry) && strcmp(actual, buf) == 0;
}

/*
    Compares a command to a string that contains a command an an argument.
*/
bool cmdcmptwo(const char *actual, char user_entry[]) {
    int i = strlen(user_entry) -1;
    while (i >= 0 && user_entry[i] == ' ') {
        user_entry[i--] = 0;
    }
    char buf[strlen(user_entry)+1];
    char buf1[strlen(user_entry)+1];
    int pos;
    int scanned = sscanf(user_entry, "%s %s%n", buf, buf1, &pos);
    return scanned == 2 && pos == strlen(user_entry) && strcmp(actual, buf) == 0;
}
/*
    Fetches the next
*/
int get_next_command(char *buf){
    static char *input_signal = "> ";
    static int input_size;
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
        if (cmdcmp(INFO_COMMAND, buf)) return_code = INFO_CODE;
        else if (cmdcmp(DIR_COMMAND, buf)) return_code = DIR_CODE;
        else if (cmdcmptwo(CD_COMMAND, buf)) return_code = CD_CODE;
        else if (cmdcmptwo(GET_COMMAND, buf)) return_code = GET_CODE;

    }
    return return_code;

}

void convert_to_upper(char *text) {
    int i;
    for (i = 0; i < strlen(text); i++) {
        text[i] = toupper(text[i]);
    }
}

void run_main_loop() {
    static char buf[MAX_BUF];


    while (true) {
        int user_command = get_next_command(buf);
        if (user_command == 0) {
            break;
        } else {
            switch(user_command) {
                case INVALID_CODE: {
                    write(STDOUT_FILENO, INVALID_COMMAND, strlen(INVALID_COMMAND));
                    break;
                }
                case INFO_CODE: {
                    print_fat32_device_info();
                    break;
                }
                case DIR_CODE: {
                    print_directory_details();
                    break;
                }
                case CD_CODE: {
                    char dir[strlen(buf)];
                    sscanf(buf, "%s %s", dir, dir);
                    convert_to_upper(dir);
                    change_current_directory(dir);
                    break;
                }
                case GET_CODE: {
                    char filename[strlen(buf)];
                    sscanf(buf, "%s %s", filename, filename);
                    convert_to_upper(filename);
                    get_file_from_current_directory(filename);
                    break;
                }
                default: {
                    write(STDOUT_FILENO, "Valid command!\n", 15);
                    break;
                }
            }
        }

    }
}

