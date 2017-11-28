#include "30.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>


int main(int argc, char** argv)
{
    if (argc > 2)
    {
        printf("Usage: %s [key]\n", argv[0]);
        return 0;
    }
    
    msg_t buffer;
    buffer.type = getuid()+1;
    int message_queue_id;

    if (argc == 1)
    {
        key_t key = (key_t) buffer.type;
        message_queue_id = msgget(key, 0);
    }
    else
    {
        key_t key = (key_t) atoi(argv[1]);
        message_queue_id = msgget(key, 0);
    }
 
    if (message_queue_id == -1)
    {
        perror("msgget");
        return EXIT_FAILURE;
    }

    while (fgets(buffer.text, sizeof(buffer.text), stdin))
    {
        if (msgsnd(message_queue_id, &buffer, strlen(buffer.text) + 1, 0) == -1)
        {
            perror("msgsnd");
            return EXIT_FAILURE;
        }
    }
}
