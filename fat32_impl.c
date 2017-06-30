#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "fat32.h"
#include "fat32_impl.h"


void set_root_dir(int fd) {
    long root_dir_sectors = ((bs->BPB_RootEntCnt * 32) + (bs->BPB_BytesPerSec - 1)) / bs->BPB_BytesPerSec;
    assert(root_dir_sectors == 0);
    long root_cluster = bs->BPB_RootClus;
    long first_data_sector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (long)bs->BPB_FATSz32);
    // printf("The root cluster is at %lu\n", root_cluster);
    // printf("The first data sector is at %lu\n", first_data_sector);


    long first_cluster_sector = ((root_cluster - 2) * bs->BPB_SecPerClus) + first_data_sector;
    long first_cluster_sector_bytes = first_cluster_sector * bs->BPB_BytesPerSec;

    lseek(fd, first_cluster_sector_bytes, SEEK_SET);

    printf("%lu ss \n", first_cluster_sector_bytes);
    curr_dir = malloc(sizeof(struct fat32DE_struct));
    read(fd, curr_dir, sizeof(struct fat32DE_struct));
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

long get_directory_byte_number() {

    long first_data_sector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (long)bs->BPB_FATSz32);


    
    long cluster_number = ((long)curr_dir->DIR_FstClusHI);
    cluster_number = cluster_number << 8;
    cluster_number = cluster_number | curr_dir->DIR_FstClusLO;
    long fat_sec_num = ((cluster_number) * bs->BPB_SecPerClus) + first_data_sector;

    return ((fat_sec_num) * bs->BPB_BytesPerSec);
}

void print_directory_details() {
    assert(curr_dir != 0);
    print_info(bs->BS_VolLab, BS_VolLab_LENGTH);
    printf("%s: ", printBuf);
    print_info(curr_dir->DIR_Name, DIR_Name_LENGTH);
    printf("%s\n\n", printBuf);


    long file_byte_position = get_directory_byte_number();
    long read_size = bs->BPB_BytesPerSec * bs->BPB_SecPerClus;
    char contents[read_size];

    // printf("%lu ss \n", file_byte_position);
    lseek(fd, file_byte_position, SEEK_SET);
    int chars_read = read(fd, contents, read_size);
    if (chars_read < 0) {
        perror("Couldn't read from sector");
        exit(EXIT_FAILURE);
    }
    // printf("Read %d characters \n", chars_read);
    fat32DE *listing;
    listing = (fat32DE *) contents;
    listing++;
    while ( listing->DIR_Name[0] ){
        int dir_name_valid = validate_dir_name(listing->DIR_Name[0]);
        if ( dir_name_valid && (listing->DIR_Attr & ATTR_HIDDEN) == 0 ){

            convert_entry_name(listing->DIR_Name);
            if ((listing->DIR_Attr & ATTR_DIRECTORY) > 0) {
                printf("%-16s %d/\n", printBuf, listing->DIR_FileSize);
            } else {
                printf("%-16s %d\n", printBuf, listing->DIR_FileSize);
            }
        }
        listing++;
    }
}



void change_current_directory(char *newdir) {

}