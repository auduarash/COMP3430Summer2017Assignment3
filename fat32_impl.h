#ifndef FAT32_IMPL_H
#define FAT32_IMPL_H

#include "fat32.h"

void load_and_validate_bpb_params();

void set_root_dir_file_entry();

void print_directory_details();

void change_current_directory(char *newdir);

void print_fat32_device_info();

void open_device(char *drive_location); 

void get_file_from_current_directory(char *f_name);
#endif