#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <path> <arg1> <argv2> ...\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    char* path = argv[1];
    char* args[argc];

    for (int i = 1; i < argc; i++)
    {
        args[i-1] = argv[i];
    }

    args[argc-1] = NULL;

    pid_t pid = fork();
    
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_SUCCESS);
    }
    else if (pid == 0)
    {    
        if (execv(path, args) == -1)
        {
            perror("execv");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int status;
        
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
        {
            printf ("Return code: %hhd\n", (char) WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Terminated by signal %d\n", WTERMSIG(status));
        }
        else
        {
            printf ("Hmmm...\n");
        }

        exit(EXIT_SUCCESS);
    }   
}
