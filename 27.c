#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

const int BUF_SIZE = 80;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 0;
    }

    // check file exist
    // check grep exist

    char buf[BUF_SIZE];
    
    if (snprintf(buf, BUF_SIZE, "grep -c '^$' %s ", argv[1]) < 0)
    {
        perror("cmd line");
        return 1;
    }

    FILE* fp = popen(buf, "r");

    if (!fp)
    {
        perror("popen");
        return 1;
    }

    int num;

    if (fscanf(fp, "%d", &num) == 1)
    {
        printf ("%d empty lines in %s\n", num, argv[1]);
    }
    else if (feof(fp))
    {
        puts("Unable to count lines");
        return 1;
    }
    else
    {
        perror("get the result");
        return 1;
    }
}
