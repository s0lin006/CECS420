/*
*   CECS420 - OS Design
*   Project 3 - spksp.c
*   Name: Shan Lin
*   Due: 03.24.2015 @ 11:59pm
*/

/*-----------------LOG---------------------------
03.??.2015
    -Set up child and thread creation. Should be working
03.16.2015
    -Takes in arg[1] and splits into path and key
    -process creation, search directory, and search file functions 
        -working properly
03.17.2015
    -Multithreading
        -able to pass multiple arguments to function in thread_create
    -Started printting function. Issue wiht
03.18.2015
    -Managed to somewhat fix multiple printing in output.txt. Process creations
     seemed to be the problem. Need to look into once near ocmpletion.
        
TODO: Bounded buffer?
        -Linked lists?
TODO: need to look at thread id. Currently, it's only printing/using one 
      thread id - instead of one thread per txt file.
TODO: Valgrind outputs "still reachable" leak in path_key and main?
TODO: FIX PRINTING MULTIPLE. SEE OUTPUT.TXT 
        - SEEMS LIKE IT'S RUNNING AFTER ALL FUNCTIONS ARE CALLED.
----------------END LOG------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>

#define true 1
#define false 0
#define bool int

#define MAXLINESIZE 1024
#define MAXOUTSIZE 2048

#define TRACE { printf("Running line: %d\n", __LINE__); }

int max_buffer = 0;

void set_buffer(int buffer_request)
{
    max_buffer = buffer_request;
}

bool check_buffer(int buffer_to_check)
{
    if(buffer_to_check <= max_buffer)
    {
        return true;
    }
    else
    {
        return false;
    }
}

struct thread_argument
{
    char file_path[MAXLINESIZE];
    char key[25];
    char file_name[50];
    FILE *fp_out;
};

struct node
{
    char *file_name;
    char *line_text;
    int line_number;
    
    struct node *prev;
    struct node *next;
}*line, *head, *last;

/*
struct buffer
{
    int size;
    struct buffer head;
    struct buffer last;
}*list;
*/



void insert(char *file_name, char *line_text, int line_number)
{
    struct node *var, *temp;
    var = (struct node*)malloc(sizeof(struct node));
        var->file_name = file_name;
        var->line_text = line_text;
        var->line_number = line_number;
    if(head == NULL)
    {
        head = var;
        head->prev = NULL;
        head->next = NULL;
        last = head;
    }
    else
    {
        last = head;
        while(last != NULL)
        {
            temp = last;
            last = last->next;
        }
        last = var;
        temp->next = last;
        last->prev = temp;
        last->next = NULL;
    }
}

void delete_node()
{
    struct node *temp;
    temp = last;
    if(temp->prev == NULL)
    {
        free(temp->file_name);
        free(temp->line_text);
        free(temp);
        head = NULL;
        last = NULL;
    }
    else
    {
        last = temp->prev;
        last->next = NULL;
        free(temp->file_name);
        free(temp->line_text);
        free(temp);
    }
}

void delete_all()
{
    struct node *n;
    while(head)
    {
        n = head->next;
        free(head->file_name);
        free(head->line_text);
        free(head);
        head = n;
    }
}

void find_head()
{
    if(head == NULL)
    {
        fprintf(stderr, ">>>List is empty\n");
    }
    else
    {
        while(head->prev)
        {
            head = head->prev;
        }
    }
}

void display()
{
    struct node *temp;
    temp = head;
    if(temp == NULL)
    {
        fprintf(stderr, "List is empty\n");
    }
    while(temp != NULL)
    {
        printf("->%s\n", temp->file_name);
        temp = temp->next;
    }
}

void print_to_file(char *file_name, int linecount, char *str_temp, FILE *fp_out)
{
    fprintf(fp_out, "%s:%d:%s", file_name, linecount, str_temp);
    //printf("%s:%d:%s", file_name, linecount, str_temp);
}

//void search_file(char *file_path, char *key, char *file_name)
void *search_file(void *arguments)
{
    struct thread_argument *targ = (struct thread_argument*)arguments;
    
    char line[MAXOUTSIZE];
    /*
    //9 possible combinations of keys and white space.
    //use this for now, fix later.
    char key1[] = "\t";
    char key2[] = "\n";
    char key3[] = " ";
    char key4[] = "NULL";
    char *temp1, *temp2, *temp3;
    */
    int linecount = 0;
    
    char *str1, *saveptr1, str_temp[MAXOUTSIZE] = "";
    
    FILE *fp_in;
    fp_in = fopen(targ->file_path, "r");
    
    if(fp_in == NULL)
    {
        fprintf(stderr, "FILE CANNOT BE READ\n");
        return 0;
    }
    else
    {
        while(!feof(fp_in))
        {
            linecount = linecount + 1;
            
            if(fgets(line, MAXOUTSIZE, fp_in))
            {
                strcpy(str_temp, line);
                str1 = strtok_r(line, " \n\t", &saveptr1);
                while(str1 != NULL)
                {
                    //if(strcmp(str1, key) == 0) //case sensitive?
                    if((strcasecmp(str1, targ->key) == 0)) //case insensitive?
                    {
                        print_to_file(targ->file_name, linecount, str_temp, targ->fp_out);
                        break;
                    }
                    //printf(">>>%s\n", str1);
                    str1 = strtok_r(NULL, " \n\t", &saveptr1);
                }
            }//end fgets
        }//end feof loop
    }
    fclose(fp_in);
    return NULL;
}

void search_dir(char *path, char *key, FILE *fp_out)
{
    //printf("Path passed is: %s\n", path);
    //struct thread_argument *args = (struct thread_argument*)malloc(sizeof(struct thread_argument));
    struct thread_argument *args = malloc(sizeof(*args));
    
    args->fp_out = fp_out;
    
    struct dirent *entry;
    DIR *d = opendir(path);
    
    pthread_t thread1;
    int iret1;
    
    char file_path[MAXLINESIZE];
    
    if(d == 0)
    {
        perror("opendir");
        return;
    }
    while((entry = readdir(d)) != 0)
    {
        if(entry->d_type == DT_REG)
        {
            if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
            {
                continue;
            }
            else
            {
                strcpy(file_path, path);
                strcat(file_path, "/");
                strcat(file_path, entry->d_name);
                
                strcpy(args->file_path, file_path);
                strcpy(args->key, key);
                strcpy(args->file_name, entry->d_name);
                
                //args->file_path = file_path;
                //args->key = key;
                //args->file_name = entry->d_name;
                
                //creates thread
                iret1 = pthread_create(&thread1, NULL, search_file, (void*)args);
                
                //printf(">>>Thread id = %lu\n", thread1);
                
                if(iret1)
                {
                    fprintf(stderr, "ERROR - pthread_create() return code: %d\n", iret1);
                    exit(EXIT_FAILURE);
                }
                
                //wait for thread to exit
                pthread_join(thread1, NULL);
                
                //printf("path is: %s\n", file_path);
                //search_file(file_path, key, entry->d_name);
            }//end
        }//end entry type regular if statement
    }//end while
    //printf(">>>END THREAD: %lu\n", thread1);
    closedir(d);
    free(args);
}

void process_creation(char *path, char *key, FILE *fp_out)
{
    //printf("process creation %s\n", path);
    pid_t n;
    
    n = fork();
    if(n < 0 )  //error
    {
        fprintf(stderr, "Fork Failed\n");
        exit(-1);
    }
    else if(n == 0) //child
    {
        //printf("Child created pid = %d\n", getpid());
        //printf("PATH: %s\n", path);
        search_dir(path, key, fp_out);
        exit(0);
    }
    wait(NULL);
    //printf("Child Compete parent id = %d\n", getpid());
    /*
    else    //parent
    {
        wait(NULL);
        //printf("Chiled Compete parent id = %d\n", getpid());
        exit(0);
    }
    */
}

void path_key(char *input, FILE *fp_out)
{
    int count = 0;
    FILE *fp_in;
    char path[MAXLINESIZE];
    char key[25];
    
    fp_in = fopen(input, "r");
    
    if(fp_in == NULL)
    {
        printf("\nFile cannot be read\n");
        fclose(fp_in);
        exit(0);
    }
    else
    {
        fscanf(fp_in, "%s %s", path, key);
        while(!feof(fp_in))
        {
            count = count +1;
            //printf("path key%d\n", count);
            process_creation(path, key, fp_out);
            fscanf(fp_in, "%s%s", path, key);
        }
    }
    fclose(fp_in);
}

int main( int argc, char* argv[])
{
    //char s[MAXLINESIZE] = "/home/shan/Desktop/project3/commandFile.txt";
    FILE *fp_out;
    fp_out = fopen("output.out", "w");
    
    if(fp_out == NULL)
    {
        perror("fopen");
        fclose(fp_out);
        //return 0;
    }
    else
    {
        path_key(argv[1], fp_out);
    }
    
    fclose(fp_out);
    return 0;
}
