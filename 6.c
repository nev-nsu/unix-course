#include <sys/select.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct 
{
    int fd;
    size_t lines_count;
    size_t offset[100];
    size_t size[100];
} FileTable;

const int BUFFER_SIZE = 6;

FileTable* read_file(const char* name)
{
    FileTable* file = (FileTable*) malloc(sizeof(FileTable));

    if (!file)
    {
        return NULL;
    }
    
    file->fd = open(name, O_RDONLY);

    if (file->fd == -1)
    {
        free(file);
        return NULL;
    }

    file->lines_count = 1;
    file->offset[0] = 0;
    char buf[BUFFER_SIZE];
    size_t i = 0, line_offset = 0;
    int nbytes = read(file->fd, buf, BUFFER_SIZE);

    while (nbytes > 0)
    {
        for (const char* ch = buf; ch < buf + BUFFER_SIZE; ch++)
        {
            i++;

            if (*ch == '\n')
            {
                file->size[file->lines_count-1] = i;
                line_offset += i;
                file->offset[file->lines_count] = line_offset;
                file->lines_count++;
                i = 0;
            }
        }

        nbytes = read(file->fd, buf, BUFFER_SIZE);
    }   

    if (nbytes == -1)
    {
        close(file->fd);
        free(file);
        return NULL;
    }

    printf("File size: %d lines\n", file->lines_count);
    return file;
}

int print_file(FileTable* ft)
{
    if (lseek(ft->fd, 0, SEEK_SET) == -1)
    {
        return -1;
    }

    char buf[BUFFER_SIZE];
    int nbytes = read(ft->fd, buf, BUFFER_SIZE);

    while (nbytes > 0)
    {
        write(STDIN_FILENO, buf, nbytes);  
        nbytes = read(ft->fd, buf, BUFFER_SIZE);
    }

    return nbytes;
}

int get_request()
{
    printf("Enter line number (0 for exit) right now: ");
    fflush(stdout);

    fd_set read_fds, write_fds, except_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&except_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0; 
    
    int code = select(STDIN_FILENO + 1, &read_fds, &write_fds, &except_fds, &timeout);

    if (code == 1)
    {
        int res;

        if (scanf("%d", &res) == EOF)
        {
            return -1;
        }
        else
        {
            return res;
        }
    }
    else
    {
        return (code == 0)? -2 : -1;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 0;
    }

    FileTable* file = read_file(argv[1]);

    if (!file)
    {
        perror("Can't read the input file");
        return 0;
    }
    
    int string_number;

    do 
    {  
        string_number = get_request();

        if (string_number == -1)
        {
            puts("Bad line number, aborted.\nERROR");
        }
        else if (string_number == -2)
        {
            puts ("TIMEOUT");
            print_file(file);
            close(file->fd);
            free(file);
            return 0;
        }
        else if (file->lines_count < string_number) 
        {
            puts("Line not found\nERROR");
            break;
        }
        else if (string_number > 0)
        {
            size_t len = file->size[string_number-1];

            for (size_t i = 0; i < len; i += BUFFER_SIZE)
            {
                char buf[BUFFER_SIZE];
                size_t chunk = (len - i > BUFFER_SIZE)? BUFFER_SIZE : len - i;  

                if (lseek(file->fd, file->offset[string_number-1] + i, SEEK_SET) == -1 || read(file->fd, buf, chunk) != chunk)
                {
                    perror("Reading error");
                    puts("ERROR");
                    close(file->fd);
                    free(file);
                    return 1;
                }
                
                write(STDIN_FILENO, buf, chunk);
            }
            
            puts("OK");
        }
    } 
    while (string_number > 0);

    close(file->fd);
    free(file);
    return 0;
}
