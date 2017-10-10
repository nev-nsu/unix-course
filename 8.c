#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

const int BUFFER_SIZE = 256;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 0;
    }

    char* filename = argv[1];
    int fd = open(filename, O_RDWR);

    if (fd == -1)
    {
        perror("Can't open the file");
        return 1;
    }

    char buf[BUFFER_SIZE];
    
    if (snprintf(buf, BUFFER_SIZE, "vim %s", filename) < 0)
    {
        close(fd);
        perror("Can't create command line");
        return 1;
    }
   
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLK, &fl) == -1) 
    {
        close(fd);
        if (errno == EACCES || errno == EAGAIN) 
        {
            printf("%s have already been locked\n", filename);
            return 0;
        } 
        else 
        {
            perror("Can't lock the file");
            return 1;
        }
    } 

    int vim_res = system(buf);
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    int unlock_res = fcntl(fd, F_SETLK, &fl);
    close(fd);
    
    if (vim_res == -1)
    {
        perror("Can't start the editor");
    } 
    else if (unlock_res == -1)
    {
        perror("Can't unlock the file");
    }
    else 
    {
        printf("Finished with exit code: %d.\n", vim_res);
        return vim_res;
    }

    return 1;
}

