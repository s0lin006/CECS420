#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <dirent.h>

#define MESSAGESIZE 128


struct message_s
{
    long type;
    char content[MESSAGESIZE];
};

void search_file(char *path, char *key, char *name, FILE *fp_out)
{
    FILE *fp_in;
    fp_in = fopen(path, "r");
    char line[1028] = "";
    char nkey1[MESSAGESIZE] = " ";
    char nkey2[MESSAGESIZE] = " ";
    char nkey3[MESSAGESIZE] = " ";
    char *temp, *temp2, *temp3;
    int linecount = 0;
    
    strcat(nkey1, key);
    strcat(nkey1, " ");
    
    strcat(nkey2, key);
    strcat(nkey2, "\t");
    
    strcat(nkey3, key);
    strcat(nkey3, "\n");
    
    //read in data from in file
    if (fp_in ==NULL)
    {
        printf("\nFile cannot be read\n");
    }
    else
    {
        //line_count = 0;
        while(!feof(fp_in))
        {
            linecount = linecount + 1;
            if (fgets(line, 1028, fp_in))
            {
                temp = strstr(line, nkey1);
                temp2 = strstr(line, nkey2);
                temp3 = strstr(line, nkey3);
                if(temp || temp2 || temp3)
                {
                    //printf("%s:%d:%s:%s", name, linecount, key, line);
                    fprintf(fp_out, "%s:%d:%s:%s", name, linecount, key, line);
                }
                /*
                else if(temp2)
                {
                    printf("%s:%d:%s:%s", path, linecount, key, line);
                }
                else if(temp3)
                {
                    printf("%s:%d:%s:%s", path, linecount, key, line);
                }
                */
            }
        }
    }//end else
    
    fclose(fp_in);
}

void search_dir(char *dir, char *key, FILE *fp_out)
{
    struct dirent *entry;
    DIR *d = opendir(dir);
    
    char str[MESSAGESIZE] = "";
    
    if(d == 0)
    {
        perror("opendir");
        return;
    }
    while ((entry = readdir(d)) != 0)
    {
        strcpy(str, "");
        if(!strcmp(entry->d_name, "."))
        {   continue;   }
        else if(!strcmp(entry->d_name, ".."))
        {   continue;   }
        else
        {
            strcat(str, dir);
            strcat(str, "/");
            strcat(str, entry->d_name);
            search_file(str, key, entry->d_name, fp_out);
        }
    }
    closedir(d);
}

int main(void)
{
    fprintf(stderr, "!!!!!!SERVER!!!!!\n");
    
    struct message_s message_k;
    struct message_s message_p;
    
    FILE *fp_out;
    fp_out = fopen("student_out.txt", "w");
    
    int message_queue_id;
    key_t key;
    
    
    if ((key = ftok("ks_client.c", 1)) == -1)
    {
        perror("ftok");
        exit(1);
    }

    if ((message_queue_id = msgget(key, 0644)) == -1)
    {
        perror("msgget");
        exit(1);
    }

    if ((msgrcv(message_queue_id, &message_k, MESSAGESIZE, 0, 0) == -1) || (msgrcv(message_queue_id, &message_p, MESSAGESIZE, 0, 0) == -1))
    {
        perror("msgrcv");
        exit(1);
    }
    
    
    search_dir(message_p.content, message_k.content, fp_out);

    if (msgctl(message_queue_id, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(1);
    }
    
    fclose(fp_out);

    return 0;
}

