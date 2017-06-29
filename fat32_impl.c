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
    printf("The root cluster is at %lu\n", root_cluster);
    printf("The first data sector is at %lu\n", first_data_sector);


    long first_cluster_sector = ((root_cluster - 2) * bs->BPB_SecPerClus) + first_data_sector;
    long first_cluster_sector_bytes = first_cluster_sector * bs->BPB_BytesPerSec;

    lseek(fd, first_cluster_sector_bytes, SEEK_SET);

    curr_dir = malloc(sizeof(struct fat32DE_struct));
    read(fd, curr_dir, sizeof(struct fat32DE_struct));
    printf("Done reading bad boy\n");
    print_info(curr_dir->DIR_Name, DIR_Name_LENGTH);
    printf("Directory name: %s\n", printBuf);
}



void print_directory_details() {
    assert(curr_dir != 0);
    print_info(bs->BS_VolLab, BS_VolLab_LENGTH);
    printf("%s: ", printBuf);
    print_info(curr_dir->DIR_Name, DIR_Name_LENGTH);
    printf("%s\n\n", printBuf);


    long first_data_sector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (long)bs->BPB_FATSz32);


    
    long cluster_number = ((long)curr_dir->DIR_FstClusHI);
    printf("Cluster number %lu \n", cluster_number);
    cluster_number = cluster_number << 8;
    printf("Cluster number %lu \n", cluster_number);
    cluster_number = cluster_number | curr_dir->DIR_FstClusLO;
    printf("Cluster number %lu \n", cluster_number);
    //fat_offset *= 4;
    long fat_sec_num = ((cluster_number) * bs->BPB_SecPerClus) + first_data_sector;
    //long fat_entry_offset = fat_offset % bs->BPB_BytesPerSec;

    long file_byte_position = ((fat_sec_num) * bs->BPB_BytesPerSec);

    //printf("The fat offset is %lu\n", fat_offset);
    // printf("The fat entry offset is %lu\n", fat_entry_offset);
    printf("The fat sector number is %lu\n", fat_sec_num);
    printf("Will be reading from byte %lu\n", file_byte_position);

    long read_size = bs->BPB_BytesPerSec * bs->BPB_SecPerClus;
    char contents[read_size];

    lseek(fd, file_byte_position, SEEK_SET);
    int chars_read = read(fd, contents, read_size);
    if (chars_read < 0) {
        perror("Couldn't read from sector");
        exit(EXIT_FAILURE);
    }
    printf("Read %d characters \n", chars_read);
    fat32DE *listing;
    listing = (fat32DE *) contents;
    while (listing->DIR_Name[0] != 0) {
        if (  (listing->DIR_Attr & ATTR_HIDDEN) == 0 ){
            print_info(listing->DIR_Name, DIR_Name_LENGTH);
            printf("There is a file :%s\n", printBuf);
        }
        listing++;
    }
}