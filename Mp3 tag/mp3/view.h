#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>
#include <string.h>
#include "types.h"

#define TITLE      "TIT2"
#define ARTIST     "TPE1"
#define ALBUM      "TALB"
#define YEAR       "TYER"
#define CONTENT    "TCON"
#define COMPOSER   "TCOM"

Status view_tags(char *file_name);

int convert_size(unsigned char *size);

void display_tag(FILE *fptr,char *tag_name);

#endif