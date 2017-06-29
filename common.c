#include "common.h"



//possible user entries into terminal
const char* INFO_COMMAND = "info";
const char* DIR_COMMAND = "dir";
const char* CD_COMMAND = "cd";
const char* GET_COMMAND = "get";
const char* INVALID_COMMAND = "Invalid command!\n";



void print_info(char info[], int length) {
    int i;
    for (i = 0; i < length; i++) {
        printBuf[i] = info[i];
    }
    printBuf[length] = 0;
}