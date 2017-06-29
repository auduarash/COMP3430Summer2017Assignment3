#include <stdio.h>

#include "common.h"
#include "command_handlers.h"


void print_device_info(fat32BS *bs) {
    //Device Info
    printf("---Device Info---\n");
    print_info(bs->BS_OEMName, BS_OEMName_LENGTH);
    printf("OEM Name: %s\n", printBuf);
    print_info(bs->BS_VolLab, BS_VolLab_LENGTH);
    printf("Label: %s\n", printBuf);
    printf("Media Type: %#02x (%s)\n", bs->BPB_Media, ( bs->BPB_Media == 0xf8 ? "fixed" : "removable") );
    // long size = bs->BPB_SecPerClus; 
    long size = bs->BPB_BytesPerSec;
    size *= bs->BPB_TotSec32;
    long sizeMB = size / ( 1024 * 1024);
    double sizeGB = sizeMB / 1024.0;
    printf("Size: %lu bytes (%luMB, %.3fGB)\n", size, sizeMB, sizeGB);
    printf("Drive Number: %d (%s)\n", bs->BS_DrvNum, (bs->BS_DrvNum == 0x00 ? "floppy" : "hard disk"));

    printf("\n---Geometry---\n");
    printf("Bytes per sector: %d\n", bs->BPB_BytesPerSec);
    printf("Sectors per cluster: %d\n", bs->BPB_SecPerClus);
    printf("Total sectors: %d\n", bs->BPB_TotSec32);

    printf("\n---FS Info---\n");
    print_info(bs->BS_VolLab, BS_VolLab_LENGTH);
    printf("Volume ID: %s\n", printBuf);
    printf("Version: %d:%d\n", bs->BPB_FSVerLow, bs->BPB_FSVerHigh);
    printf("Reserved Sectors: %d\n", bs->BPB_RsvdSecCnt);
    printf("Number of FATs: %d\n", bs->BPB_NumFATs);
    printf("FAT Size: %d\n", bs->BPB_FATSz32);
    int mirrored = ( 7 & bs->BPB_ExtFlags); //last 3 bits tell number of mirrored fats
    int mirrored_enabled = (128 & bs->BPB_ExtFlags);
    if (mirrored_enabled == 0) mirrored = 0;
    printf("Mirrored FAT: %d (%s)\n", mirrored, (mirrored_enabled ? "yes" : "no"));
}


void print_current_directory(fat32BS *bs) {
    
}