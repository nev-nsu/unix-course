#include "30.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/types.h>

int message_queue_id;

void cleanup()
{
    msgctl(message_queue_id, IPC_RMID, 0);
    exit(0);
}

int main(int argc, char** argv)
{
    if (argc > 2)
    {
        printf("Usage: %s [key]\n", argv[0]);
        return 0;
    }

    if (argc == 1)
    {
        key_t key = (key_t) getuid()+1;
        message_queue_id = msgget(key, IPC_CREAT | 0600);
    }
    else
    {
        key_t key = (key_t) atoi(argv[1]);
        message_queue_id = msgget(key, IPC_CREAT | 0666);
    }
     
    if (message_queue_id == -1)
    {
        perror("msgget");
        return EXIT_FAILURE;
    }

    signal(SIGINT, cleanup);
    msg_t buffer;

    while (1)
    {
        if (msgrcv(message_queue_id, &buffer, sizeof(buffer.text), 0, 0) == -1)
        {
            perror("msgrcv");
            return EXIT_FAILURE;
        }

        printf("[%d] \"%s\"\n", buffer.type, buffer.text);
        fflush(0);
    }
}
