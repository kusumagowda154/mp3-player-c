/* edit.c */

/* Color macros for terminal output */
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edit.h"

/* Function to map command-line options to corresponding ID3 frame IDs */
char *get_tag(char *option)
{
    if(strcmp(option,"-t") == 0)
        return "TIT2";     // Title

    if(strcmp(option,"-a") == 0)
        return "TPE1";     // Artist

    if(strcmp(option,"-A") == 0)
        return "TALB";     // Album

    if(strcmp(option,"-y") == 0)
        return "TYER";     // Year

    if(strcmp(option,"-c") == 0)
        return "TCON";     // Genre
    
    if(strcmp(option,"-C") == 0)
        return "COMM";     // Comment

    return NULL;
}

/* Function to write a 4-byte integer in big-endian format */
void write_big_endian(FILE *fptr,int size)
{
    unsigned char arr[4];

    arr[0] = (size >> 24) & 0xFF;
    arr[1] = (size >> 16) & 0xFF;
    arr[2] = (size >> 8) & 0xFF;
    arr[3] = size & 0xFF;

    fwrite(arr,1,4,fptr);
}

/* Function to modify a specific ID3 frame with new data */
Status modify_tag(char *file_name,
                  char *frame_id,
                  char *new_data)
{
    FILE *src;
    FILE *temp;

    char tag[5];
    unsigned char size_buf[4];
    char flags[2];

    /* Open source MP3 file */
    src = fopen(file_name,"rb");

    if(src == NULL)
    {
        printf(RED);
        printf("ERROR : Unable to open source file\n");
        printf(RESET);
        return FAILURE;
    }

    /* Create temporary file */
    temp = fopen("temp.mp3","wb");

    if(temp == NULL)
    {
        fclose(src);
        return FAILURE;
    }

    /* Copy ID3 header */
    char header[10];

    fread(header,1,10,src);
    fwrite(header,1,10,temp);

    int found = 0;

    /* Traverse through frames */
    while(fread(tag,1,4,src) == 4)
    {
        tag[4] = '\0';

        /* Read frame size */
        fread(size_buf,1,4,src);

        int frame_size = convert_size(size_buf);

        /* Read frame flags */
        fread(flags,1,2,src);

        /* Check if current frame matches required frame */
        if(strcmp(tag,frame_id) == 0)
        {
            found = 1;

            /* Write frame ID */
            fwrite(tag,1,4,temp);

            /* Calculate and write new frame size */
            int new_size = strlen(new_data) + 1;

            write_big_endian(temp,new_size);

            /* Write existing flags */
            fwrite(flags,1,2,temp);

            /* Write encoding byte */
            fputc(0,temp);

            /* Write updated frame content */
            fwrite(new_data,
                   1,
                   strlen(new_data),
                   temp);

            /* Skip old frame data */
            fseek(src,frame_size,SEEK_CUR);
        }
        else
        {
            /* Copy unchanged frame */
            fwrite(tag,1,4,temp);
            fwrite(size_buf,1,4,temp);
            fwrite(flags,1,2,temp);

            char *buffer = malloc(frame_size);

            fread(buffer,1,frame_size,src);

            fwrite(buffer,1,frame_size,temp);

            free(buffer);
        }

        /* Stop after updating required frame */
        if(found)
            break;
    }

    /* Copy remaining data from source file */
    int ch;

    while((ch = fgetc(src)) != EOF)
    {
        fputc(ch,temp);
    }

    /* Close files */
    fclose(src);
    fclose(temp);

    /* Replace original file with modified file */
    remove(file_name);
    rename("temp.mp3",file_name);

    printf(GREEN);
    printf("INFO : MP3 Tag Updated Successfully\n");
    printf(RESET);
    return SUCCESS;
}

/* Function to validate edit option and invoke tag modification */
Status edit_tags(int argc,char *argv[])
{
    char *frame_id;

    /* Get corresponding frame ID */
    frame_id = get_tag(argv[2]);

    if(frame_id == NULL)
    {
        printf(RED);
        printf("ERROR : Invalid Edit Option\n");
        printf(RESET);
        return FAILURE;
    }

    /* Modify selected tag */
    return modify_tag(argv[4],
                      frame_id,
                      argv[3]);
}