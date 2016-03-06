#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

#include <dirent.h>

#include <sys/ipc.h>

#define MESSAGESIZE 128
#define KEYWORDSIZE 32
#define DIRPATHSIZE 128
#define LINESIZE 1024
#define RESULTSIZE 2048

#define TRACE {fprintf(stderr, "EXECUTIN LINE: %d\n", __LINE__); }

struct message_s
{
    long type;
    char content[MESSAGESIZE];
};

void search_file(char *path, char *key, char *name, FILE *fp_out)
{
    char line[LINESIZE] = "";
    char nkey1[MESSAGESIZE] = "";
    char nkey2[MESSAGESIZE] = "";
    char nkey3[MESSAGESIZE] = "";
    char *temp, *temp2, *temp3;
    int linecount = 0;
    
    strcat(nkey1, key);
    strcat(nkey1, " ");
    
    strcat(nkey2, key);
    strcat(nkey2, "\t");
    
    strcat(nkey3, key);
    strcat(nkey3, "\n");
    
    //read file
    FILE *fp_in;
    fp_in = fopen(path, "r");
    
    if(fp_in == NULL)
    {
        printf("FILE CANNOT BE READ\n");
    }
    else
    {
        while(!feof(fp_in))
        {
            linecount = linecount + 1;
            
            if(fgets(line, LINESIZE, fp_in))
            {
                temp = strstr(line, nkey1);
                temp2 = strstr(line, nkey2);
                temp3 = strstr(line, nkey3);
                
                if(temp || temp2 || temp3)
                {
                    fprintf(fp_out, "%s:%d:%s:%s", name, linecount, key, line);
                }
                else
                {
                    continue;
                }
            }
        }//end while
    }//end else
    fclose(fp_in);
}

void search_dir(char *key, char *dir, FILE *fp_out)
{
    struct dirent *entry;
    DIR *d = opendir(dir);
    
    char str[MESSAGESIZE] = "";
    
    if(d == 0)
    {
        perror("opendir");
        return;
    }
    while( (entry = readdir(d)) != 0)
    {
//TRACE;
        strcpy(str, "");
        if(!strcmp(entry->d_name, "."))
        {
            continue;
        }
        else if(!strcmp(entry->d_name, ".."))
        {
            continue;
        }
        else
        {
            strcat(str, dir);
            strcat(str, entry->d_name);
            search_file(str, key, entry->d_name, fp_out);
        }
    }//end while
}

int main(void)
{
    //used for starting server
    //NEED TO FIX!!!!!!!!!!!!!!!!!
    //system("./ks_client");
    
    //printf("------SERVER STARTING...-----\n");
    
    struct message_s message_k;
    struct message_s message_p;
    
    int message_queue_id;
    key_t key;

    strcpy(message_k.content, "");
    strcpy(message_p.content, "");
    
    FILE *fp_out;
    fp_out = fopen("student_out.txt", "w");
    
    if(fp_out == NULL)
    {
        printf("Error in opening file\n");
    }
    
    if ((key = ftok("ks_client.c", 1)) == -1)
    {
        perror("ftok");
        exit(1);
    }
  
    if ((message_queue_id = msgget(key, 0444)) == -1)
    {
        perror("msgget");
        exit(1);
    }
  
    while(1)
    {
        if ( (msgrcv(message_queue_id, &message_k, MESSAGESIZE, 0, 0) == -1) || (msgrcv(message_queue_id, &message_p, MESSAGESIZE, 0, 0) == -1) ) 
        {
            perror("msgrcv");
            exit(1);
        }
  
        //message_k.type = 1;
        //message_p.type = 1;
        //printf("message received\n");
  
        if((strcmp(message_k.content, "exit")) == 0)
        {
            //printf("exit is hit...extitng server...\n");
            break;
        }
////////////////////////////////////////////////////////////////////////////
        pid_t n;
  
        n = fork();
  
        if ( n < 0 )
        {
            fprintf(stderr, "FORK FIALED\n");
            exit(-1);
        }
        else if ( n == 0 )
        {
            //printf("child created\n");
            //printf("%s\n", message_p.content);
            search_dir(message_k.content, message_p.content, fp_out);
            exit(0);
        }
        wait(NULL);
        /*
        else
        {
            //printf("Parent%d, %d..waiting for child\n", getpid(), n);
            //search_dir(message_k.content, message_p.content, fp_out);
            wait(NULL);
            //printf("CHILED COMPLEERTE\n");
            exit(0);
        }
        */
  
    }//end forever

    if ((msgctl(message_queue_id, IPC_RMID, NULL) == -1))
    {
        perror("msgctl");
        exit(1);
    }
    fclose(fp_out);
    
    //printf("---END SERVER---\n");
    
    system("sort student_out.txt -o student_out.txt");
    
    return 0;
}
