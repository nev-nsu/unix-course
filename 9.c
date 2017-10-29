#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    printf("Parrent before\n");
    pid_t pid = fork();
    
    if (pid == -1)
    {
        perror("Parrent after (error)");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        char* const argv[] = {"cat", "9.c", NULL};
        
        if(execv("/usr/bin/cat", argv) == -1)
        {
            perror("Child in process (error)");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        printf("Parrent in process\n");
        
        if (waitpid(pid, NULL, 0) == -1)
        {
            perror("Parrent in process (error)");
            exit(EXIT_FAILURE);
        }

        printf("Parrent after\n");
        exit(EXIT_FAILURE);
    }
}
