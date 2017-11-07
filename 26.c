#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

const int BUF_SIZE = 100;

int on_data(char* data)
{
    for (char* p = data; *p != '\0'; p++)
    {
        *p = (char) toupper((unsigned char) *p);
    }

    if (printf(":%s", data) < 0)
    {
        perror("client printf");
        return -1;
    }

    fflush(NULL);
    return 0;
}

void client(FILE* fp)
{
    char buffer[BUF_SIZE];
    ssize_t nbytes;

    while (fgets(buffer, BUF_SIZE, fp))
    {
       if (on_data(buffer))
       {
           return;
       }
    }

    if (feof(fp))
    {
        printf("EOF\n");
    }
    else
    {
        perror("client fgets");
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <server_path>\n", argv[0]);
        return 0;
    }

    puts("Creating a server...");
    FILE* fp = popen(argv[1], "r");

    if (fp == NULL)
    {
        perror("popen");
        return 1;
    }

    client(fp);
    int status = pclose(fp);

    if (status == -1)
    {
        perror("pclose");
        return 1;
    }
}
