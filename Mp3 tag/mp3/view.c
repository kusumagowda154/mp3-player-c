#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "view.h"

/* ANSI color codes for formatted terminal output */
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define MAGENTA "\033[1;35m"
#define RESET   "\033[0m"

/* Convert 4-byte big-endian frame size to little endian */
int convert_size(unsigned char *size)
{
    return ((size[0] << 24) |
            (size[1] << 16) |
            (size[2] << 8)  |
             size[3]);
}

/* Validate whether the file contains an ID3 tag */
Status validate_id3(FILE *fptr)
{
    char id[4];

    rewind(fptr);

    /* Read first 3 bytes and check for "ID3" */
    fread(id,1,3,fptr);
    id[3] = '\0';

    if(strcmp(id,"ID3") != 0)
        return FAILURE;

    return SUCCESS;
}

/* Display a specific tag value */
void display_tag(FILE *fptr,char *tag_name)
{
    unsigned char size_buf[4];
    int frame_size;

    /* Read frame size */
    fread(size_buf,1,4,fptr);

    frame_size = convert_size(size_buf);

    /* Skip frame flags */
    fseek(fptr,2,SEEK_CUR);

    /* Allocate memory for frame data */
    char *data = malloc(frame_size);

    if(data == NULL)
        return;

    /* Read frame content */
    fread(data,1,frame_size,fptr);

    /* Display tag name and tag value */
    printf(BLUE "%-15s" RESET " : " MAGENTA "%s" RESET "\n",
       tag_name,
       data + 1);

    free(data);
}

/* View and display all supported MP3 tags */
Status view_tags(char *file_name)
{
    FILE *fptr;

    char frame_id[5];
    unsigned char version[2];

    /* Open MP3 file in binary read mode */
    fptr = fopen(file_name,"rb");

    if(fptr == NULL)
    {
        printf(RED);
        printf("ERROR : Unable to open file\n");
        printf(RESET);
        return FAILURE;
    }

    /* Validate ID3 header */
    if(validate_id3(fptr) == FAILURE)
    {
        printf(RED);
        printf("ERROR : ID3 tag missing\n");
        fclose(fptr);
        printf(RESET);
        return FAILURE;
    }

    /* Read ID3 version */
    fread(version,1,2,fptr);

    /* Check for ID3v2.3 support */
    if(version[0] != 3)
    {
        printf(RED);
        printf("ERROR : Only ID3v2.3 supported\n");
        printf(RESET);
        fclose(fptr);
        return FAILURE;
    }

    /* Display heading */
    printf(CYAN);
    printf("\n=====================================\n");
    printf("      MP3 TAG READER AND EDITOR\n");
    printf("=====================================\n\n");
    printf(RESET);

    printf(YELLOW);
    printf("ID3 Version : 2.3.0\n\n");
    printf(RESET);

    /* Skip remaining header bytes */
    fseek(fptr,5,SEEK_CUR);

    /* Read frames one by one */
    while(fread(frame_id,1,4,fptr) == 4)
    {
        frame_id[4] = '\0';

        /* Check frame type and display corresponding tag */
        if(strcmp(frame_id,"TIT2") == 0)
            display_tag(fptr,"Title");

        else if(strcmp(frame_id,"TPE1") == 0)
            display_tag(fptr,"Artist");

        else if(strcmp(frame_id,"TALB") == 0)
            display_tag(fptr,"Album");

        else if(strcmp(frame_id,"TYER") == 0)
            display_tag(fptr,"Year");

        else if(strcmp(frame_id,"TCON") == 0)
            display_tag(fptr,"Genre");

        else if(strcmp(frame_id,"COMM") == 0)
            display_tag(fptr,"Comment");

        else
        {
            /* Skip unsupported frames */
            unsigned char size_buf[4];
            int frame_size;

            fread(size_buf,1,4,fptr);

            frame_size = convert_size(size_buf);

            fseek(fptr,2,SEEK_CUR);
            fseek(fptr,frame_size,SEEK_CUR);
        }

        /* Stop if end of file reached */
        if(feof(fptr))
            break;
    }

    /* Display success message */
    printf(GREEN);
    printf("\n=====================================\n");
    printf("Tag Details Displayed Successfully\n");
    printf("=====================================\n");

    fclose(fptr);

    printf(RESET);

    return SUCCESS;
}