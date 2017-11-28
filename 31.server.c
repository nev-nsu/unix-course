#include "30.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/msg.h>
#include <sys/types.h>

const int MAX_SUBSCIBERS = 100;

int message_queue_id;

void cleanup()
{
    msgctl(message_queue_id, IPC_RMID, 0);
    exit(0);
}

int main()
{
    srand(time(0));
    int key = rand();
    message_queue_id = msgget(key, IPC_CREAT | 0666);
     
    if (message_queue_id == -1)
    {
        perror("msgget");
        return EXIT_FAILURE;
    }

    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
    printf("Use key %d to subscribe\n", key);
    msg_t buffer_from;
    buffer_from.text[0] = '1';
    int subscribers[MAX_SUBSCIBERS], count = 0;

    while (fgets(buffer_from.text + 1, sizeof(buffer_from.text) - 1, stdin))
    {
        msg_t buffer_to;

        while (msgrcv(message_queue_id, &buffer_to, sizeof(buffer_to.text), 1, IPC_NOWAIT) != -1)
        {
            int id = atoi(buffer_to.text);
            
            if (id != 0)
            {
                if (count == MAX_SUBSCIBERS)
                {
                    puts("Error: too many subscribers. The new subscriber will be ignored");
                }
                else
                {
                    subscribers[count++] = id;
                }
            }
        }
        
        if (errno != ENOMSG)
        {
            perror("msgrcv");
            msgctl(message_queue_id, IPC_RMID, 0);
            return EXIT_FAILURE;
        }
        
        int len = strlen(buffer_from.text) + 1;

        for (int i = 0; i < count; i++)
        {
            buffer_from.type = subscribers[i];
            msgsnd(message_queue_id, &buffer_from, len, 0);
        }
    }
    
    buffer_from.text[0] = '0';

    for (int i = 0; i < count; i++)
    {
        buffer_from.type = subscribers[i];
        msgsnd(message_queue_id, &buffer_from, 1, 0);
    }

    while (count)
    {
        msg_t buffer_to;
        
        if (msgrcv(message_queue_id, &buffer_to, sizeof(buffer_to.text), 1, 0) != -1)
        {
            int id = atoi(buffer_to.text);
            
            if (id < 0)
            {
                printf("Client %d was unsubscribed\n", -id);
                count--;    
            }
        }

    }

    msgctl(message_queue_id, IPC_RMID, 0);
}
