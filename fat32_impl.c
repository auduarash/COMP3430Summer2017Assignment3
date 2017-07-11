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
#include "fat32_calculations.h"
#include "utilities.h"

static fat32DE *curr_dir = NULL; //the current directory in the navigation blah
static fat32BS *bs = NULL; //bpb holder
static int fd = 0; //file descriptor for directory

void validate_bpb_params() {
    assert(bs != NULL);
    uint64_t root_dir_sectors = calculate_root_dir_sectors(bs);
    assert(root_dir_sectors == FAT32_ROOT_DIR_SECTORS);
    if (root_dir_sectors != FAT32_ROOT_DIR_SECTORS) {
        perror("Invalid fat type. Please enter a fat32 Volume");
        exit(EXIT_FAILURE);
    }
    uint64_t cluster_count = calculate_cluster_count(bs);
    assert(cluster_count >= MIN_FAT32_CLUSTER_COUNT);
    if (cluster_count < MIN_FAT32_CLUSTER_COUNT) {
        perror("Invalid fat type. Please enter a fat32 Volume\n");
        exit(EXIT_FAILURE);
    }
    uint64_t sector_510_bytes = 510;
    uint16_t fat32_signature;
    read_bytes_into_variable(fd, sector_510_bytes, &fat32_signature, sizeof(uint16_t));
    printf("I got signature %d\n", fat32_signature);
    assert(fat32_signature == 0xAA55);
    if (fat32_signature != 0xAA55) {
        perror("Invalid fat32 signature");
        exit(EXIT_FAILURE);
    }

}

void load_and_validate_bpb_params() {
    bs = (fat32BS *) malloc(sizeof(struct fat32BS_struct));
    assert(bs != NULL);
    read_bytes_into_variable(fd, BPB_ROOT, bs, sizeof(struct fat32BS_struct));
    validate_bpb_params();
}




void open_device(char *drive_location) {
    fd = open(drive_location, O_RDONLY);
    if (fd == -1) {
        perror("open_device");
        exit(EXIT_FAILURE);
    }
}


void set_root_dir_file_entry() {
    uint64_t first_cluster_sector_bytes = get_byte_location_from_cluster_number(bs, bs->BPB_RootClus);
    curr_dir = (fat32DE *) malloc(sizeof(struct fat32DE_struct));
    assert(curr_dir != NULL);
    read_bytes_into_variable(fd, first_cluster_sector_bytes, curr_dir, sizeof(struct fat32DE_struct));
}

void print_directory_details() {
    assert(curr_dir != NULL); //Make sure this has been initialized
    assert(bs != NULL);
    char printBuf[MAX_BUF];
    long read_size = bs->BPB_BytesPerSec * bs->BPB_SecPerClus; // TODO Refactor
    char contents[read_size];

    uint64_t next_cluster = convert_high_low_to_cluster_number(curr_dir->DIR_FstClusHI, curr_dir->DIR_FstClusLO);
    uint64_t file_byte_position = get_byte_location_from_cluster_number(bs, next_cluster);
    read_byte_location_into_buffer(fd, file_byte_position, contents, read_size);

    fat32DE *listing = (fat32DE *) contents;
    memcpy(curr_dir, listing, sizeof(struct fat32DE_struct));

    print_chars_into_buffer(printBuf, bs->BS_VolLab, BS_VolLab_LENGTH);
    printf("Volume: %s\n", printBuf);
    print_chars_into_buffer(printBuf, curr_dir->DIR_Name, DIR_Name_LENGTH);
    printf("Directory Name: %s\n\n", printBuf);
    int total_lines = get_number_of_lines_in_entry(bs);
    while (true) {
        int lines_read = 0; //We shouldn't read more than a certain number of lines per sector

        //We print out all directory entries in the current cluster
        while ( listing->DIR_Name[0] && (lines_read++) < total_lines ){
            bool entry_valid = is_printable_entry(listing);
            if ( entry_valid ){

                char *printableEntryName = convert_file_entry_name(listing->DIR_Name);
                char *file_end = (is_attr_directory(listing->DIR_Attr) ? "/" : "");
                printf("%-16s %d%s\n", printableEntryName, listing->DIR_FileSize, file_end);
                free(printableEntryName);
            }
            listing++;
        }
        //Get the next cluster entry for the file
        uint64_t cluster_entry_bytes = calculate_fat_entry_for_cluster(bs, next_cluster);
        read_bytes_into_variable(fd, cluster_entry_bytes, &next_cluster, 8);
        next_cluster = next_cluster & 0x0FFFFFFF;
        if (next_cluster >= 0x0FFFFFF7) break; //break if there is no more cluster

        //update to the next cluster for the directory
        file_byte_position = get_byte_location_from_cluster_number(bs, next_cluster);
        read_byte_location_into_buffer(fd, file_byte_position, contents, read_size);
        listing = (fat32DE *) contents;
    } 
}




void change_current_directory(char *newdir) {
    uint64_t read_size = bs->BPB_BytesPerSec * bs->BPB_SecPerClus;
    char contents[read_size];

    uint64_t next_cluster = convert_high_low_to_cluster_number(curr_dir->DIR_FstClusHI, curr_dir->DIR_FstClusLO);
    uint64_t file_byte_position = get_byte_location_from_cluster_number(bs, next_cluster);
    read_byte_location_into_buffer(fd, file_byte_position, contents, read_size);

    fat32DE *listing = (fat32DE *) contents;
    int total_lines = get_number_of_lines_in_entry(bs);
    //TODO Refactor
    while (true) {
        int lines_read = 0; //We shouldn't read more than a certain number of lines per sector
        //We check all directory entries in the current cluster
        while ( listing->DIR_Name[0] && (lines_read++) < total_lines ){
            if ( listing_is_navigable_directory(listing) ) {
                char *compBuf = convert_file_entry_name(listing->DIR_Name);
                if (strcmp(newdir, compBuf) == 0) {
                    free(compBuf);
                    memcpy(curr_dir, listing, sizeof(struct fat32DE_struct));
                    return;
                } else {
                    free(compBuf);
                }
            }
            listing++;
        }
        //Get the next cluster entry for the file
        uint64_t cluster_entry_bytes = calculate_fat_entry_for_cluster(bs, next_cluster);
        read_bytes_into_variable(fd, cluster_entry_bytes, &next_cluster, sizeof(uint64_t));
        next_cluster = next_cluster & 0x0FFFFFFF;
        if (next_cluster >= 0x0FFFFFF7) break; //break if there is no more cluster

        //update to the next cluster for the directory
        file_byte_position = get_byte_location_from_cluster_number(bs, next_cluster);
        read_byte_location_into_buffer(fd, file_byte_position, contents, read_size);
        listing = (fat32DE *) contents;
    }
    printf("Directory %s doesn't exist\n", newdir);

}


void print_fat32_device_info() {
    char printBuf[MAX_BUF];
    printf("---Device Info---\n");
    print_chars_into_buffer(printBuf, bs->BS_OEMName, BS_OEMName_LENGTH);
    printf("OEM Name: %s\n", printBuf);
    print_chars_into_buffer(printBuf, bs->BS_VolLab, BS_VolLab_LENGTH);
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
    print_chars_into_buffer(printBuf, bs->BS_VolLab, BS_VolLab_LENGTH);
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


void download_file(fat32DE *listing, char *f_name) {
    printf("Downloading %s\n", f_name);
    uint64_t size = listing->DIR_FileSize;
    uint64_t next_clus = convert_high_low_to_cluster_number(listing->DIR_FstClusHI,listing->DIR_FstClusLO);
    FILE *fp;
    fp = fopen(f_name, "w");
    while (size > 0) {
        if (next_clus >= 0x0FFFFFF8) {
            printf("Reached end of file \n");
            break;
        } else {
            //we are reading this amount of characters
            uint64_t to_read = 4096;
            if (size < to_read) {
                to_read = size;
            }
            size -= to_read;

            //get the next cluster and read into a file
            long byte_location = get_byte_location_from_cluster_number(bs, next_clus);
            read_byte_location_into_file(fd, fp, byte_location, to_read);
            
            //get the next cluster
            next_clus = calculate_fat_entry_for_cluster(bs, next_clus);
            read_bytes_into_variable(fd, next_clus, &next_clus, sizeof(uint64_t));
            next_clus = next_clus & 0x0FFFFFFF;
        }
    }
    printf("File write successful\n");
    fclose(fp);
}


void get_file_from_current_directory(char *f_name) {

    long read_size = bs->BPB_BytesPerSec * bs->BPB_SecPerClus;
    char contents[read_size];
    uint64_t next_cluster = convert_high_low_to_cluster_number(curr_dir->DIR_FstClusHI, curr_dir->DIR_FstClusLO);
    uint64_t file_byte_position = get_byte_location_from_cluster_number(bs, next_cluster);
    read_byte_location_into_buffer(fd, file_byte_position, contents, read_size);

    fat32DE *listing = (fat32DE *) contents;
    //TODO Refactor
    while (true) {
        while (listing->DIR_Name[0]) {
            if ( listing_is_readable_file(listing) ) {
                char *compBuf = convert_file_entry_name(listing->DIR_Name);
                if (strcmp(f_name, compBuf) == 0) {
                    // memcpy(curr_dir, listing, sizeof(struct fat32DE_struct));
                    // printf("Found file %s\n", printBuf);
                    // printf("File start %d %d\n", listing->DIR_FstClusHI, listing->DIR_FstClusLO);
                    download_file(listing, f_name);
                    return;
                }
                free(compBuf);
            }
            listing++;
        }
        //Get the next cluster entry for the file
        uint64_t cluster_entry_bytes = calculate_fat_entry_for_cluster(bs, next_cluster);
        read_bytes_into_variable(fd, cluster_entry_bytes, &next_cluster, sizeof(uint64_t));
        next_cluster = next_cluster & 0x0FFFFFFF;
        if (next_cluster >= 0x0FFFFFF7) break; //break if there is no more cluster

        //update to the next cluster for the directory
        file_byte_position = get_byte_location_from_cluster_number(bs, next_cluster);
        read_byte_location_into_buffer(fd, file_byte_position, contents, read_size);
        listing = (fat32DE *) contents;
    }
    printf("File %s doesn't exist in current directory\n", f_name);
}