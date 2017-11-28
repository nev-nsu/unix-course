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
 
    msg_t buffer;
    int count = 0;

    while (msgrcv(message_queue_id, &buffer, sizeof(buffer.text), 0, 0) != -1)
    {
        if (buffer.type == 1)
        {
            int id = atoi(buffer.text);

            if (id > 0)
            {
                count++;
            }
            else if (id < 0)
            {
                count--;
                
                if (count == 0)
                {
                    break;
                }
            }
        }
        else
        {
            printf("[%d] \"%s\"\n", buffer.type, buffer.text);
        }
    }
    
    msgctl(message_queue_id, IPC_RMID, 0);
    
    if (count != 0)
    {
        perror("msgrcv");
        return EXIT_FAILURE;
    }
  
    return EXIT_SUCCESS;
}
