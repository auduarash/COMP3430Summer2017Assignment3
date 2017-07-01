#include <stdio.h>

#include "common.h"
#include "command_handlers.h"
#include "fat32_impl.h"


void print_device_info() {
    //Device Info
    print_fat32_device_info();
}


void print_current_directory() {
    print_directory_details();
}



void change_directory(char *new_dir) {
    change_current_directory(new_dir);
}


void get_file(char *f_name){
    get_file_from_current_directory(f_name);
}