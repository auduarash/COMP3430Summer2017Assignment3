#ifndef FAT32_IMPL_H
#define FAT32_IMPL_H

#include "fat32.h"

void set_root_dir(int fd);

void print_directory_details();

void change_current_directory(char *newdir);
#endif