#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fat32_calculations.h"


uint64_t get_byte_location_from_cluster_number(fat32BS *bs, uint64_t clus_num) {
    uint64_t first_data_sector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (uint64_t)bs->BPB_FATSz32);
    if (clus_num == 0) {
        //0 is not representative of a cluster so we have to ensure we are
        //checking the root cluster
        clus_num = bs->BPB_RootClus;
    }
    uint64_t cluster_sector = ((clus_num-2) * bs->BPB_SecPerClus) + first_data_sector;
    uint64_t byte_location = cluster_sector * bs->BPB_BytesPerSec;
    return byte_location;
}

char *convert_file_entry_name(char entry_name[]) {
    char *retBuf = (char *) malloc(sizeof(char) * DIR_Name_LENGTH);
    int i; int p_count = 0;
    for (i = 0; i < 8 && entry_name[i] != ' '; i++) {
        retBuf[i] = entry_name[i];
        p_count++;
    }
    if (entry_name[8] != ' ') retBuf[p_count++] = '.';
    for (i = 8; i < 11 && entry_name[i] != ' '; i++) {
        retBuf[p_count++] = entry_name[i];
    }
    retBuf[p_count++] = 0;
    return retBuf;
}

bool is_dir_name_valid(char *dir_name) {
    return ! ((uint8_t)(dir_name[0]) == 0x05 || (uint8_t)(dir_name[0]) == 0xE5);
}


bool is_printable_entry(fat32DE * d) {
    return is_dir_name_valid(d->DIR_Name) //name should be valid
        // && (d->DIR_Attr & ATTR_READ_ONLY) == 0 //not read only
        && (d->DIR_Attr & ATTR_HIDDEN) == 0  // not hidden
        && (d->DIR_Attr & ATTR_VOLUME_ID) == 0; //not the root directory
}

/*
    Uses the high bit and low bit to calculate the next cluster number.
    This function will return 2 if the 
*/
uint64_t convert_high_low_to_cluster_number(uint8_t high, uint8_t low) {
    uint64_t clus_num = high << 8; //Shift by 8 bits
    clus_num = clus_num | low;
    return clus_num;
}


uint64_t calculate_fat_entry_for_cluster(fat32BS *bs, uint64_t next_clus) {
    uint64_t fat_offset = next_clus * 4l;
    uint64_t fat_sec_num = bs->BPB_RsvdSecCnt + (fat_offset / bs->BPB_BytesPerSec);
    uint64_t fat_ent_offset = fat_offset % bs->BPB_BytesPerSec;
    uint64_t next_listing = (fat_sec_num * bs->BPB_BytesPerSec) + fat_ent_offset;
    return next_listing;
}


uint64_t get_number_of_lines_in_entry(fat32BS *bs) {
    return num_bytes_in_cluster(bs) / 32;
}

bool is_attr_directory(uint8_t dir_attr) {
    return (dir_attr & ATTR_DIRECTORY) != false;
}

bool is_attr_hidden(uint8_t dir_attr) {
    return (dir_attr & ATTR_HIDDEN) != false;
}

//is the directory the root directory?
bool is_attr_root_dir(uint8_t dir_attr) {
    return (dir_attr & ATTR_VOLUME_ID) != false;
}

bool listing_is_navigable_directory(fat32DE *listing) {
    uint8_t dir_attr = listing->DIR_Attr;
    return is_dir_name_valid(listing->DIR_Name) && 
        is_attr_directory(dir_attr) && //is a directory
        !is_attr_hidden(dir_attr) && //not hidden
        !is_attr_root_dir(dir_attr); // not the root directory
}

bool listing_is_readable_file(fat32DE *listing) {
    uint8_t dir_attr = listing->DIR_Attr;
    return is_dir_name_valid(listing->DIR_Name) && 
        ! is_attr_directory(dir_attr)  && //is not a directory
       ! is_attr_hidden(dir_attr); // is not hidden
}


uint64_t num_bytes_in_cluster(fat32BS *bs) {
    return bs->BPB_BytesPerSec * (uint64_t)bs->BPB_SecPerClus;
}


uint64_t calculate_root_dir_sectors(fat32BS *bs) {
    uint64_t rds = ((bs->BPB_RootEntCnt * (uint64_t)32) + (bs->BPB_BytesPerSec - 1)) / bs->BPB_BytesPerSec;
    return rds;
}


uint64_t calculate_cluster_count(fat32BS *bs) {
    uint64_t rds = calculate_root_dir_sectors(bs);
    uint64_t fatsz;
    if (bs->BPB_FATSz16 != 0) {
        fatsz = bs->BPB_FATSz16;
    } else {
        fatsz = bs->BPB_FATSz32;
    }
    uint64_t totSec;
    if(bs->BPB_TotSec16 != 0) {
        totSec = bs->BPB_TotSec16;
    } else {
        totSec = bs->BPB_TotSec32;
    }
    //Not order of casting. Must be 64 bit
    uint64_t dataSec = totSec - (bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (uint64_t)fatsz) + rds);
    uint64_t countofClusters = dataSec / bs->BPB_SecPerClus;
    return countofClusters;
}