#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "fat32.h"
#include "fat32_impl.h"
#include "command_handlers.h"
#include "program_logic.h"






/**
    main.c
    
    Implementation of a fat32 file reader.

    @author Abdul-Rasheed Audu
    @since 24/06/2017
*/
int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s {usbdevice location}\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    open_device(argv[1]);
    load_bpb_params();

    set_root_dir_file_entry();

    run_main_loop();
    
    return 0;
}