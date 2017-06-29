#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "fat32.h"
#include "fat32_impl.h"


void set_root_dir(fat32BS *bs, int fd) {
    long root_dir_sectors = ((bs->BPB_RootEntCnt * 32) + (bs->BPB_BytesPerSec - 1)) / bs->BPB_BytesPerSec;
    assert(root_dir_sectors == 0);
    long root_cluster = bs->BPB_RootClus;
    long first_data_sector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (long)bs->BPB_FATSz32);
    printf("The first data sector is at %lu\n", first_data_sector);


    long first_cluster_sector = ((root_cluster - 2) * bs->BPB_SecPerClus) + first_data_sector;
    long first_cluster_sector_bytes = first_cluster_sector * bs->BPB_BytesPerSec;

    lseek(fd, first_cluster_sector_bytes, SEEK_SET);

    fat32DE *de = malloc(sizeof(struct fat32DE_struct));
    read(fd, de, sizeof(struct fat32DE_struct));
    printf("Done reading bad boy\n");
    print_info(de->DIR_Name, DIR_Name_LENGTH);
    printf("Directory name: %s\n", printBuf);
}