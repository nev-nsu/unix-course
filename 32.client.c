#include "30.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/types.h>

int send_control_message(int mid, int id)
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

    if (send_control_message(message_queue_id, pid) == -1)
    {
        return EXIT_FAILURE;
    }
   
    msg_t buffer;
    buffer.type = pid;

    for (int i = 0; i < 10; i++)
    {
        sleep(1);
        int len = snprintf(buffer.text, sizeof(buffer.text), "%d", i);
        
        if (len < 0 || msgsnd(message_queue_id, &buffer, len + 1, 0) == -1)
        {
            break;
        }
    }

    send_control_message(message_queue_id, -pid);
}
