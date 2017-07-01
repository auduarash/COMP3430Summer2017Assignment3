#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "fat32.h"
#include "fat32_impl.h"

static fat32DE *curr_dir; //the current directory in the navigation blah
static fat32BS *bs; //bpb holder
static int fd; //file descriptor for directory

void set_root_dir_file_entry() {
    long root_dir_sectors = ((bs->BPB_RootEntCnt * 32) + (bs->BPB_BytesPerSec - 1)) / bs->BPB_BytesPerSec;
    assert(root_dir_sectors == 0);
    long root_cluster = bs->BPB_RootClus;
    long first_data_sector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (long)bs->BPB_FATSz32);

    long first_cluster_sector = ((root_cluster-2) * bs->BPB_SecPerClus) + first_data_sector;
    long first_cluster_sector_bytes = first_cluster_sector * bs->BPB_BytesPerSec;

    lseek(fd, first_cluster_sector_bytes, SEEK_SET);

    // printf("%lu ss \n", first_cluster_sector_bytes);
    curr_dir = malloc(sizeof(struct fat32DE_struct));
    read(fd, curr_dir, sizeof(struct fat32DE_struct));
    // printf("%d %d\n", curr_dir->DIR_FstClusLO, curr_dir->DIR_FstClusHI);
}

int validate_dir_name(char d) {
    if ((uint8_t)d == 0xE5 || (uint8_t)d == 0x05) return 0;
    else return 1;
}

void convert_entry_name(char entry_name[]) {
    int i; int p_count = 0;
    for (i = 0; i < 8 && entry_name[i] != ' '; i++) {
        printBuf[i] = entry_name[i];
        p_count++;
    }
    if (entry_name[8] != ' ') printBuf[p_count++] = '.';
    for (i = 8; i < 11 && entry_name[i] != ' '; i++) {
        printBuf[p_count++] = entry_name[i];
    }
    printBuf[p_count++] = 0;
}

long convert_high_low_to_cluster_number(long high_bit, long low_bit) {
    long first_data_sector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (long)bs->BPB_FATSz32);
    long res = high_bit;
    res = res << 8;
    res = res | low_bit;
    return ((res-2) * bs->BPB_SecPerClus + first_data_sector) * bs->BPB_BytesPerSec;
}

long get_directory_byte_number() {
    long high_bit = curr_dir->DIR_FstClusHI, low_bit = curr_dir->DIR_FstClusLO;
    /**/
    if ( ((curr_dir->DIR_Attr & ATTR_VOLUME_ID) > 1) || (high_bit == 0 && low_bit == 0) ) {
        high_bit = 0; 
        low_bit = 2;
    }
    return convert_high_low_to_cluster_number(high_bit, low_bit);
}

void print_directory_details() {
    assert(curr_dir != 0);
    print_info(bs->BS_VolLab, BS_VolLab_LENGTH);
    printf("Volume: %s\n", printBuf);


    long file_byte_position = get_directory_byte_number();
    long read_size = bs->BPB_BytesPerSec * bs->BPB_SecPerClus;
    char contents[read_size];

    lseek(fd, file_byte_position, SEEK_SET);
    int chars_read = read(fd, contents, read_size);
    if (chars_read < 0) {
        perror("Couldn't read from sector");
        exit(EXIT_FAILURE);
    }
    // printf("Read %d characters \n", chars_read);
    fat32DE *listing;
    listing = curr_dir = (fat32DE *) contents;

    print_info(curr_dir->DIR_Name, DIR_Name_LENGTH);
    printf("Directory Name: %s\n\n", printBuf);
    
    while ( listing->DIR_Name[0] ){
        int dir_name_valid = validate_dir_name(listing->DIR_Name[0]);
        if ( dir_name_valid && (listing->DIR_Attr & ATTR_HIDDEN) == 0 && (listing->DIR_Attr & ATTR_VOLUME_ID) == 0){

            convert_entry_name(listing->DIR_Name);
            if ((listing->DIR_Attr & ATTR_DIRECTORY) > 0) {
                printf("%-16s %d/\n", printBuf, listing->DIR_FileSize);
            } else {
                printf("%-16s %d\n", printBuf, listing->DIR_FileSize);
            }
            // printf("%#04x %#04x\n", listing->DIR_FstClusHI, listing->DIR_FstClusLO);
        }
        listing++;
    }
}

bool listing_is_navigable_directory(fat32DE *listing) {
    return (listing->DIR_Attr & ATTR_DIRECTORY) > 0 && //is a directory
        (listing->DIR_Attr & ATTR_HIDDEN) == 0 && //is not hidden
        (listing->DIR_Attr & ATTR_VOLUME_ID) == 0; //is not the root directory
}


void change_current_directory(char *newdir) {
    long read_size = bs->BPB_BytesPerSec * bs->BPB_SecPerClus;
    char contents[read_size];
    long dir_bytes = get_directory_byte_number();
    lseek(fd, dir_bytes, SEEK_SET);

    int chars_read = read(fd, contents, read_size);
    if (chars_read < 0) {
        perror("error reading directory");
        exit(EXIT_FAILURE);
    }

    fat32DE *listing = (fat32DE *) contents;
    //TODO Refactor
    while (listing != NULL && listing->DIR_Name[0]) {
        int dir_name_valid = validate_dir_name(listing->DIR_Name[0]);;
        if ( dir_name_valid && listing_is_navigable_directory(listing) ) {
            print_info(listing->DIR_Name, DIR_Name_LENGTH);
            sscanf(printBuf, "%s", printBuf);
            if (strcmp(newdir, printBuf) == 0) {
                memcpy(curr_dir, listing, sizeof(struct fat32DE_struct));
                printf("Changing to %d %d\n", listing->DIR_FstClusLO, listing->DIR_FstClusHI);
                return;
            }
        }
        listing++;
    }
    printf("Directory %s doesn't exist\n", newdir);

}


void print_fat32_device_info() {
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


void load_bpb_params() {
    int array_size = sizeof(fat32BS) / sizeof(char);
    char bs_bpb[ array_size  ];

    int seek = lseek(fd, 0, SEEK_SET);
    if (seek < 0) {
        perror("seek");
        exit(EXIT_FAILURE);
    }
    int read_size = read(fd, bs_bpb, array_size);
    if (read_size < 0) {
        perror("read bpb");
        exit(EXIT_FAILURE);
    }
    bs = malloc(sizeof(fat32BS));
    memcpy(bs, bs_bpb, sizeof(fat32BS));
}




void open_device(char *drive_location) {
    fd = open(drive_location, O_RDONLY);
    if (fd == -1) {
        perror("open_device");
        exit(EXIT_FAILURE);
    }

}