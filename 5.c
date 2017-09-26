#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct 
{
    int fd;
    size_t length;
    size_t offset[100];
    size_t size[100]
} FileTable;

FileTable* read_file(const char* name)
{
    int fd = open(name, O_RDONLY);

    if (fd == -1)
    {
        return NULL;
    }

    char ch;
    FileTable* file = (FileTable*) malloc(sizeof(FileTable));

    if (!file)
    {
        return NULL;
    }

    file->fd = fd;
    file->length = 1;
    file->offset[0] = 0;
    size_t i = 0;

    while (read(fd, &ch, 1))
    {
        i++;

        if (ch == '\n')
        {
            off_t offset = lseek(fd, 0L, SEEK_CUR);

            if (offset == -1)
            {
                free(file);
                return NULL;
            }

            file->offset[file->length] = offset;
            file->size[file->length-1] = i;
            file->length++;
            i = 0;
        }
    }   

    printf("File size: %d lines\n", file->length);
    return file;
}

int get_request()
{
    printf("Enter line number (0 for exit): ");
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

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        puts("Usage: reader <filename>");
        return 0;
    }

    FileTable* file = read_file(argv[1]);

    if (!file)
    {
        perror("Can't read the input file");
        exit(1);
    }

    int string_number;

    do 
    {  
        string_number = get_request();

        if (string_number == 0)
        {
            close(file->fd);
            free(file);
            return 0;
        }
        else if (string_number < 0)
        {
            puts("ERROR\nBad line number, aborted.");
            close(file->fd);
            free(file);
            return 1;
        }
        
        else if (file->length < string_number) // куда скастуется?
        {
            puts("ERROR\nLine not found");
            close(file->fd);
            free(file);
            return 0; // какой тут должен быть код?
        }
        
        char buf[256];
        size_t len = file->size[string_number-1]; 
                
        if (lseek(file->fd, file->offset[string_number-1], SEEK_SET) == -1 || read(file->fd, buf, len) == -1)
        {
            puts("ERROR");
            perror("Reading error");
            close(file->fd);
            free(file);
            exit(1);
        }
        
        puts("OK");
        write(STDIN_FILENO, buf, len);
    } 
    while (string_number);
}
