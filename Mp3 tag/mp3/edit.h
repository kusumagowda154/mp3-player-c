#ifndef EDIT_H
#define EDIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/* Main edit function */
Status edit_tags(int argc, char *argv[]);

/* Get frame ID from edit option */
char *get_tag(char *option);

/* Convert Big Endian size to Integer */
int convert_size(unsigned char *size);

/* Modify selected tag */
Status modify_tag(char *file_name,
                  char *frame_id,
                  char *new_data);

/* Convert Integer to Big Endian and write */
void write_big_endian(FILE *fptr, int size);

#endif