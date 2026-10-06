#include <stdio.h>
#include <string.h>
#include "view.h"
#include "edit.h"

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define MAGENTA  "\033[1;35m"
#define RESET   "\033[0m"


void print_usage(void)
{
    printf("\n");
    printf("MP3 TAG READER & EDITOR\n\n");

    printf("VIEW:\n");
    printf("./a.out -v sample.mp3\n\n");

    printf("EDIT:\n");
    printf("./a.out -e -t \"TITLE\" sample.mp3\n");
    printf("./a.out -e -a \"ARTIST\" sample.mp3\n");
    printf("./a.out -e -A \"ALBUM\" sample.mp3\n");
    printf("./a.out -e -y \"YEAR\" sample.mp3\n");
    printf("./a.out -e -c \"Genre\" sample.mp3\n");
    printf("./a.out -e -C \"COMMENT\" sample.mp3\n");
}

int validate_mp3(char *file_name)
{
    char *ext;

    ext = strrchr(file_name,'.');

    if(ext == NULL)
        return FAILURE;

    if(strcmp(ext,".mp3") != 0)
        return FAILURE;

    return SUCCESS;
}

int main(int argc,char *argv[])
{
    if(argc < 3)
{
    printf(RED "ERROR : Insufficient Arguments\n" RESET);
    print_usage();
    return FAILURE;
}

    if(strcmp(argv[1],"-v") == 0)
    {
        if(argc != 3)
        {
            printf(RED);
            printf("ERROR : Invalid arguments\n");
            printf(RESET);
            return FAILURE;
        }

        if(validate_mp3(argv[2]) == FAILURE)
        {
            printf(RED);
            printf("ERROR : Not an MP3 file\n");
            printf(RESET);
            return FAILURE;
        }

        return view_tags(argv[2]);
    }

    else if(strcmp(argv[1],"-e") == 0)
    {
        if(argc != 5)
        {
            printf(RED);
            printf("ERROR : Invalid edit command\n");
            print_usage();
            printf(RESET);
            return FAILURE;
        }

        if(validate_mp3(argv[4]) == FAILURE)
        {
            printf(RED);
            printf("ERROR : Not an MP3 file\n");
            printf(RESET);
            return FAILURE;
        }

        return edit_tags(argc,argv);
    }

    else
    {
        printf(RED);
        printf("ERROR : Invalid option\n");
        print_usage();
        printf(RESET);
        return FAILURE;
    }
}