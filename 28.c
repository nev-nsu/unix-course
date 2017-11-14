#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

const int NUM_COUNT = 100;

int main()
{
    time_t now = time(NULL);
   
    if (now == (time_t)-1)
    {
        perror("time");
        return EXIT_FAILURE;
    }

    srand((unsigned int) now);
    FILE* fp[2];

    if (p2open("sort -h", fp) == -1)
    {
        puts("p2open failed");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_COUNT; i++)
    {
        int num = rand() % 100;
        
        if (fprintf(fp[0], "%d\n", num) < 0)
        {
            perror("fprintf");
            p2close(fp);
            return EXIT_FAILURE;
        }
    }

    fflush(0);

    if (fclose(fp[0]))
    {
        perror("fclose");
        return EXIT_FAILURE;
    }

    char buffer[8];

    while (fgets(buffer, 8, fp[1]))
    {
        puts(buffer);
    }

    if (!feof(fp[1]))
    {
        perror("fgets");
        return EXIT_FAILURE;
    }

    if (p2close(fp) == -1)
    {
        puts("p2close failed");
        return EXIT_FAILURE;
    }
}
