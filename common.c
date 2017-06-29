#include "common.h"

void print_info(char info[], int length) {
    int i;
    for (i = 0; i < length; i++) {
        printBuf[i] = info[i];
    }
    printBuf[length] = 0;
}