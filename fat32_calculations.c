#include <stdlib.h>
#include <string.h>
#include "fat32_calculations.h"


uint64_t get_byte_location_from_cluster_number(fat32BS *bs, uint64_t clus_num) {
    uint64_t first_data_sector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * (uint64_t)bs->BPB_FATSz32);

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

bool is_dir_name_valid(char first) {
    return ! ((uint8_t)first == 0x05 || (uint8_t)first == 0xE5);
}


bool is_printable_entry(fat32DE * d) {
    return is_dir_name_valid(d->DIR_Name[0]) //name should be valid
        && (d->DIR_Attr & ATTR_READ_ONLY) == 0 //not read only
        && (d->DIR_Attr & ATTR_HIDDEN) == 0  // not hidden
        && (d->DIR_Attr & ATTR_VOLUME_ID) == 0; //not the root directory
}


uint64_t convert_high_low_to_cluster_number(uint64_t high, uint64_t low) {
    high = high << 8;
    high = high | low;
    if (high == 0) high = 2;
    return high;
}


uint64_t calculate_fat_entry_for_cluster(fat32BS *bs, uint64_t next_clus) {
    uint64_t fat_offset = next_clus * 4;
    uint64_t fat_sec_num = bs->BPB_RsvdSecCnt + (fat_offset / bs->BPB_BytesPerSec);
    uint64_t fat_ent_offset = fat_offset % bs->BPB_BytesPerSec;
    uint64_t next_listing = (fat_sec_num * bs->BPB_BytesPerSec) + fat_ent_offset;
    return next_listing;
}


uint64_t get_number_of_lines_in_entry(fat32BS *bs) {
    return bs->BPB_BytesPerSec / 32;
}

bool listing_is_navigable_directory(fat32DE *listing) {
    return is_dir_name_valid(listing->DIR_Name[0]) &&
        (listing->DIR_Attr & ATTR_DIRECTORY) > 0 && //is a directory
        (listing->DIR_Attr & ATTR_HIDDEN) == 0 && //is not hidden
        (listing->DIR_Attr & ATTR_VOLUME_ID) == 0; //is not the root directory
}

bool listing_is_readable_file(fat32DE *listing) {
    return is_dir_name_valid(listing->DIR_Name[0]) &&
        (listing->DIR_Attr & ATTR_DIRECTORY) == 0 && //is not a directory
        (listing->DIR_Attr & ATTR_HIDDEN) == 0 && //is not hidden
        (listing->DIR_Attr & ATTR_VOLUME_ID) == 0; //is not the root directory
}