/*
*   CECS420 - OS Design
*   Project 2 - ks_client.c
*   Name: Shan Lin
*   Due: 02.19.2015 @ 11:59pm
*   >Extended: 02:24:2015 @ 11:59pm
*/

/*-----------------LOG------------------
02.14.2015
    Initial version - Used class example 'sender.c' as starting point.
02.15.2015
    Created 2 message struct inorder to send two arguments to server.
02.??.2015
    Cannot recieve message from server.
    
TODO: Look into only using only one message to send two arguments to server.
TODO: Send message from server back to client.

*/
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
    //fprintf(stderr, "---RUNNING CLIENT---\n");
    
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

    if( (msgsnd(message_queue_id, &message_k, MESSAGESIZE, 0) == -1) || (msgsnd(message_queue_id, &message_p, MESSAGESIZE, 0) == -1) )
    {
        perror("Error in msgsnd");
    }
    
    //fprintf(stderr, "---END CLIENT---\n");
    
    return 0;
}
