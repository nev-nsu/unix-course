#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>

const int BUF_SIZE = 100;
const int sleep_time = 10;

void client(int fildes[2])
{
    close(fildes[1]);
    char buffer[BUF_SIZE];
    ssize_t nbytes;

    while ((nbytes = read(fildes[0], buffer, BUF_SIZE)) > 0)
    {
        for (int i = 0; i < nbytes; i++)
        {
            buffer[i] = (char) toupper((unsigned char) buffer[i]);
        }

        if (write(STDOUT_FILENO, buffer, nbytes) == -1)
        {
            perror("client write");
            exit(EXIT_FAILURE);
        }
    }

    close(fildes[0]);
    
    if (nbytes == -1)
    {
        perror("client read");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

void server(int fildes[2])
{
    close(fildes[0]);
    if (write(fildes[1], "Hello\n", 6) == -1)
    {
        perror("server write");
        exit(EXIT_FAILURE);
    }
    
    char buffer[BUF_SIZE];
    ssize_t nbytes;

    while ((nbytes = read(STDIN_FILENO, buffer, BUF_SIZE)) > 0)
    {
        if (write(fildes[1], buffer, nbytes) == -1)
        {
            perror("server write");
            exit(EXIT_FAILURE);
        }
    }

    close(fildes[1]);
    exit(EXIT_SUCCESS);
}

int main()
{
    int fildes[2];
    
    if (pipe(fildes) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t client_pid = fork();

    switch (client_pid)
    {
        case -1:
            perror("fork 1");
            exit(EXIT_FAILURE);
        case 0:
            client(fildes);
        default:
            printf("Child process %d was created (client)\n", (unsigned int) client_pid);
    }

    pid_t server_pid = fork();
    
    switch (server_pid)
    {
        case -1:
            perror("fork 2");
            exit(EXIT_FAILURE);
        case 0:
            server(fildes);
        default:
            printf("Child process %d was created (server)\n", (unsigned int) server_pid);
    }

    close(fildes[0]);
    close(fildes[1]);
    waitpid(client_pid, NULL, 0);
    waitpid(server_pid, NULL, 0);
}
