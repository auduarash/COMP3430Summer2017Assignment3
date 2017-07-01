#ifndef COMMON_H
#define COMMON_H

#include "fat32.h"

//maximum buffer size for reading in input
#define MAX_BUF 10000

//possible user entries into terminal
const char * INFO_COMMAND;
const char * DIR_COMMAND;
const char * CD_COMMAND;
const char * GET_COMMAND;
const char * INVALID_COMMAND;
//string representatiions of the code
#define QUIT_CODE 0
#define INFO_CODE 1
#define DIR_CODE 2
#define CD_CODE 3
#define GET_CODE 4
#define INVALID_CODE -1

char printBuf[MAX_BUF];


void print_info(char info[], int length);
#endif
