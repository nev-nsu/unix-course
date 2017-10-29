#include <sys/select.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <libgen.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct 
{
    char* data;
    size_t size;
    size_t lines_count;
    size_t offset[100];
    size_t lines_size[100];
} FileTable;

const int BUFFER_SIZE = 6;

FileTable* read_file(const char* name)
{
    FileTable* file = (FileTable*) malloc(sizeof(FileTable));

    if (!file)
    {
        return NULL;
    }
    
    int fd = open(name, O_RDONLY);

    if (fd == -1)
    {
        free(file);
        return NULL;
    }

    struct stat st;

    if (fstat(fd, &st) < 0)
    {
        close(fd);
        free(file);
        return NULL;
    }

    file->size = (size_t) st.st_size;
    file->data = (char*) mmap(NULL, file->size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    
    if (file->data == MAP_FAILED)
    {
        free(file);
        return NULL;
    }

    file->lines_count = 1;
    file->offset[0] = 0;
    size_t i = 0, line_offset = 0;
        
    for (const char* ch = file->data; ch < file->data + file->size; ch++)
    {
        i++;

        if (*ch == '\n')
        {
            file->lines_size[file->lines_count-1] = i;
            line_offset += i;
            file->offset[file->lines_count] = line_offset;
            file->lines_count++;
            i = 0;
        }
    }

    printf("File size: %d lines\n", file->lines_count);
    return file;
}

int print_file(FileTable* ft)
{
    write(STDIN_FILENO, ft->data, ft->size);  
}

int get_request()
{
    printf("Enter line number (0 for exit) right now: ");
    fflush(stdout);

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0; 
    
    int code = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

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
            munmap(file->data, file->size);
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
            size_t len = file->lines_size[string_number-1];
            write(STDIN_FILENO, file->data + file->offset[string_number-1], len);
            puts("OK");
        }
    } 
    while (string_number > 0);

    munmap(file->data, file->size);
    free(file);
    return 0;
}
