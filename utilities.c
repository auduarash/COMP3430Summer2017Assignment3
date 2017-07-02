#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "utilities.h"



void read_byte_location_into_buffer(int fd, uint64_t byte_position, char buffer[], uint64_t chars_to_read) {
    assert(fd != -1);
    lseek(fd, byte_position, SEEK_SET);
    int chars_read = read(fd, buffer, chars_to_read);
    if (chars_read < 0) {
        perror("Error reading cluster");
        exit(EXIT_FAILURE);
    }
}


void read_byte_location_into_file(int fd, FILE *fp, uint64_t byte_position, uint64_t chars_to_read) {
    char buffer[chars_to_read];
    assert(fd != -1);
    lseek(fd, byte_position, SEEK_SET);
    int chars_read = read(fd, buffer, chars_to_read);
    if (chars_read < 0) {
        perror("Error reading cluster");
        exit(EXIT_FAILURE);
    }
    fwrite(buffer, sizeof(char), sizeof(char)*chars_read, fp);
}

//TODO issue might be here
void read_bytes_into_variable(int fd, uint64_t byte_position, void *destination, int num_bytes_to_read) {
    assert(fd != -1);
    lseek(fd, byte_position, SEEK_SET);
    int bytes_read = read(fd, destination, num_bytes_to_read);
    if (bytes_read < 0) {
        perror("Error reading bytes");
        exit(EXIT_FAILURE);
    }
}




void print_chars_into_buffer(char dest[], char info[], int length) {
    int i;
    for (i = 0; i < length; i++) {
        dest[i] = info[i];
    }
    dest[length] = 0;
}