
#ifndef COMMAND_HANDLERS_H
#define COMMAND_HANDLERS_H

#include "fat32.h"

void print_device_info();

void print_current_directory();

void change_directory(char *new_dir);

#endif