#include "30.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/types.h>

int subscribe(int mid, int id)
{
    msg_t buffer;
    buffer.type = 1;
    int len = snprintf(buffer.text, sizeof(buffer.text), "%d", id);

    if (len < 0 || msgsnd(mid, &buffer, len, 0) == -1)
    {
        perror("msgsnd");
        return -1;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <key>\n", argv[0]);
        return EXIT_SUCCESS;
    }
   
    int message_queue_id = msgget(atoi(argv[1]), 0);
 
    if (message_queue_id == -1)
    {
        perror("msgget");
        return EXIT_FAILURE;
    }

    int pid = getpid();

    if (subscribe(message_queue_id, pid) == -1)
    {
        return EXIT_FAILURE;
    }
   
    msg_t buffer;
    
    while (msgrcv(message_queue_id, &buffer, sizeof(buffer.text), pid, 0) != -1)
    {
        if (buffer.text[0] == '0')
        {
            return EXIT_SUCCESS;
        }

        printf(buffer.text+1);
        fflush(stdout);
    }
}
