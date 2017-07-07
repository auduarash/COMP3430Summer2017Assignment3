#ifndef FAT32_CALCULATIONS_H
#define FAT32_CALCULATIONS_H

#include <stdbool.h>
#include "fat32.h"


uint64_t get_byte_location_from_cluster_number(fat32BS *bs, uint64_t clus_num);

char *convert_file_entry_name(char entry_name[]);

bool is_printable_entry(fat32DE * d);

bool is_dir_name_valid(char *dir_name);

uint64_t convert_high_low_to_cluster_number(uint64_t high, uint64_t low);

uint64_t calculate_fat_entry_for_cluster(fat32BS *bs, uint64_t next_clus);

uint64_t get_number_of_lines_in_entry(fat32BS *bs);

bool listing_is_navigable_directory(fat32DE *listing);

bool listing_is_readable_file(fat32DE *listing);

uint64_t num_bytes_in_cluster(fat32BS *bs);

uint64_t calculate_root_dir_sectors(fat32BS *bs);

uint64_t calculate_cluster_count(fat32BS *bs);
#endif