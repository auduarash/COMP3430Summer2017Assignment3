#ifndef PROGRAM_LOGIC_H
#define PROGRAM_LOGIC_H

#include "fat32.h"

int get_next_command();

void handle_user_choice(int user_choice);

void open_device(char *drive_location); 

#endif