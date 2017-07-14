#ifndef UTILITIES_H
#define UTILITIES_H
#include <inttypes.h>
#include <stdio.h>

void read_byte_location_into_file(int fd, FILE *fp, uint64_t byte_position, uint64_t chars_to_read);

void read_byte_location_into_buffer(int fd, uint64_t byte_position, char buffer[], uint64_t chars_to_read) ;

void read_bytes_into_variable(int fd, uint64_t byte_position, void *destination, uint64_t num_bytes_to_read);

void print_chars_into_buffer(char dest[], char info[], int length);

#endif