#ifndef FAT32_IMPL_H
#define FAT32_IMPL_H

#include "fat32.h"

void set_root_dir_file_entry();

void print_directory_details();

void change_current_directory(char *newdir);

void print_fat32_device_info();

void open_device(char *drive_location); 

void load_bpb_params();
#endif