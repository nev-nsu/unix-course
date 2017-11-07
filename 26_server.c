#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>

const int BUF_SIZE = 100;

int main()
{
    if (write(STDOUT_FILENO, "Hello\n", 6) == -1)
    {
        perror("server write");
        exit(EXIT_FAILURE);
    }
    
    char buffer[BUF_SIZE];
    ssize_t nbytes;

    while ((nbytes = read(STDIN_FILENO, buffer, BUF_SIZE)) > 0)
    {
        if (write(STDOUT_FILENO, buffer, nbytes) == -1)
        {
            perror("server write");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
