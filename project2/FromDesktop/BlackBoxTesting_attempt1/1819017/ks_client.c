#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MESSAGESIZE 128
#define KEYWORDSIZE 32
#define DIRPATHSIZE 128
#define LINESIZE 1024
#define RESULTSIZE 2048


struct message_s
{
    long type;
    char content[MESSAGESIZE];
};

int main(int argc, char *argv[])
{
    fprintf(stderr, "!!!!!!!client!!!!\n");
    
    struct message_s message_k;
    struct message_s message_p;

    int message_queue_id;
    key_t key;

    if ((key = ftok("ks_client.c", 1)) == -1)
    {
        perror("ftok");
        exit(1);
    }

    if ((message_queue_id = msgget(key, 0644 | IPC_CREAT)) == -1)
    {
        perror("msgget");
        exit(1);
    }
    
    message_k.type = 1;
    message_p.type = 1;
    strcpy(message_k.content, argv[1]);
    strcpy(message_p.content, argv[2]);
    

    if((msgsnd(message_queue_id, &message_k, MESSAGESIZE, 0) == -1) || (msgsnd(message_queue_id, &message_p, MESSAGESIZE, 0) == -1))
    {
        perror("Error in msgsnd");
    }
  
    fprintf(stderr, "!!!!!!!END client!!!!\n");
  return 0;
}
