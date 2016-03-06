/*
*   CECS420 - OS Design
*   Project 4 - disksched.c
*   Name: Shan Lin
*   Due: 04.16.2015 @ 11:59pm
*/

/*-----------------LOG---------------------------
04.10.2015 - 04.14.2015
    -NOPE...restarting...
04.15.2015
    
TODO: info function
----------------END LOG------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRACE { printf("Running line: %d\n", __LINE__); }

#define SECTORSPERTRACK 200
#define TRACKSPERCYLINDER 8
#define CYLINDERS 500000
#define RPM 10000
#define PHYSICALSECTORSIZE 512
#define LOGICALBLOCKSIZE 4096
#define TRACKTOTRACKSEEK 2
#define FULLSEEK 16
#define TRANSFERRATE 1


struct node
{
    double arrival_time;
    double finish_time;
    double waiting_time;
    double sector_offset;
    int cylinder;
    int physical_sector_number;
    int surface;
    int seek_distance;
    int logical_block_number;
    int request_size;
    
    struct node *prev;
    struct node *next;
}*head, *last;

int get_psn(int lbn)
{
    int x = 8 * lbn;
    return x;
}

int get_cylinder(int psn)
{
    int x = psn/1600;
    return x;
}

double get_sector_offset(int psn)
{
    double x = psn;
    
    while(x >= 200)
    {
        x = x - 200;
    }
    
    return x;
}

int get_surface(int psn)
{
    int x = (psn/200) % 8;
    return x;
}

double get_seek_distance(int prev_seek_dist, int curr_cylinder)
{
    double x = 0;
    x = curr_cylinder - prev_seek_dist;
    if(x < 0)
    {
        x = x * -1;
    }
    return x;
}

double get_waiting(double arrival, double prev_finish)
{
    double x = 0;
    
    x = prev_finish - arrival;
    if(x < 0)
    {
        x = x * -1;
    }
    
    return x;
}

double get_finsh(double arrival, double waiting, int prev_cylinder, int curr_cylinder)
{
    double x, service, seek, rotational, transfer;
    int d;
    
    d = curr_cylinder - prev_cylinder;
    if(d < 0)
    {
        d = d * -1;
    }
    
    if(d == 0)
    {
        seek = 0;
    }
    else
    {
        seek = (0.000028 * d) + 2;
    }
    
    service = seek + rotational + transfer;
    
    x = arrival + waiting + service;
    return x;
}

void insert(double larrival_time, int llogical_block_number, int lrequest_size)
{
    struct node *var, *temp;
    var = (struct node*)malloc(sizeof(struct node));
        var->arrival_time = larrival_time;
        var->finish_time = 0;
        var->waiting_time = 0;
        var->sector_offset = 0;
        var->physical_sector_number = 0;
        var->cylinder = 0;
        var->surface = 0;
        var->seek_distance = 0;
        var->logical_block_number = llogical_block_number;
        var->request_size = lrequest_size;
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
        free(temp);
        head = NULL;
        last = NULL;
    }
    else
    {
        last = temp->prev;
        last->next = NULL;
        free(temp);
    }
}

void delete_all()
{
    struct node *n;
    while(head)
    {
        n = head->next;
        free(head);
        head = n;
    }
}

void swap(struct node *left, struct node *right)
{
    if(left->prev)
    {
        left->prev->next = right;
    }
    if(right->next)
    {
        right->next->prev = left;
    }
    left->next = right->next;
    right->prev = left->prev;
    
    right->next = left;
    left->prev = right;
}

void find_head()
{
    if(head == NULL)
    {
        printf(">>>List is empty\n");
    }
    else
    {
        while(head->prev)
        {
            head = head->prev;
        }
    }
}

void insert_sort()
{
    struct node *temp1, *temp2;
    temp1 = head->next;
    
    while((temp1->prev != NULL) && (temp1 != NULL))
    {
        temp2 = temp1;

        while((temp2->prev != NULL)&&((temp2->finish_time) < (temp2->prev->finish_time)))
        {
            swap(temp2->prev, temp2);
        }
        if(temp1->next == NULL)
        {
            break;
        }
        else
        {
            temp1 = temp1->next;
        }
    }
}

void display()
{
    struct node *temp;
    temp = head;
    if(temp == NULL)
    {
        printf("list is empty\n");
    }
    while(temp != NULL)
    {
        printf("%lf %lf %d %d\n", temp->arrival_time, temp->finish_time, temp->logical_block_number, temp->request_size);
        temp = temp->next;
    }
}

void print_to_file(FILE *fp_out)
{
    struct node *temp;
    temp = head;
    
    while(temp != NULL)
    {
        fprintf(fp_out, "%lf %lf %lf %d %d %d %lf %d\n", temp->arrival_time, temp->finish_time, temp->waiting_time, temp->physical_sector_number, temp->cylinder, temp->surface, temp->sector_offset, temp->seek_distance);
        temp = temp->next;
    }
}

void time_info()
{
    find_head();
    struct node *temp;
    temp = head;
    
    while(temp != NULL)
    {
        if(temp == head)
        {
            temp->waiting_time = 0;
            //temp->finish_time;
        }
        else
        {
            temp->waiting_time = get_waiting(temp->arrival_time, temp->prev->finish_time);
            //temp->finish_time;
        }
        temp = temp->next;
    }
}

void info()
{
    find_head();
    struct node *temp;
    temp = head;
    
    while(temp != NULL)
    {
        if(temp == head)
        {
            temp->physical_sector_number = get_psn(temp->logical_block_number);
            temp->cylinder = get_cylinder(temp->physical_sector_number);
            temp->sector_offset = get_sector_offset(temp->physical_sector_number);
            temp->surface = get_surface(temp->physical_sector_number);
            //temp->seek_distance = get_seek_distance(0, temp->cylinder);
        }
        else
        {
            temp->physical_sector_number = get_psn(temp->logical_block_number);
            temp->cylinder = get_cylinder(temp->physical_sector_number);
            temp->sector_offset = get_sector_offset(temp->physical_sector_number);
            temp->surface = get_surface(temp->physical_sector_number);
            //temp->seek_distance = get_seek_distance(temp->prev->seek_distance, temp->cylinder);
        }
        temp = temp->next;
    }
}

int main(int argc, char *argv[])
{
    FILE *fp_in;
    FILE *fp_out;
    fp_in = fopen(argv[1], "r");
    fp_out = fopen(argv[2], "w");
    
    int optional = 0;
    
    double arrival_time = 0;
    int logical_block_number = 0;
    int request_size = 0;
    
    if(fp_in == NULL || fp_out == NULL)
    {
        printf("\nFile cannot be read\n");
    }
    else
    {
        fscanf(fp_in, "%lf%d%d", &arrival_time, &logical_block_number, &request_size);
        while(!feof(fp_in))
        {
            if(argc == 5)
            {
                if(optional == atoi(argv[4]))
                {
                    break;
                }
                optional = optional + 1;
            }
            insert(arrival_time, logical_block_number, request_size);
            fscanf(fp_in, "%lf%d%d", &arrival_time, &logical_block_number, &request_size);
        }
    }
    
    find_head();
    time_info();
    
    //find_head();
    //insert_sort();
    
    find_head();
    info();
    
    find_head();
    print_to_file(fp_out);
    
    find_head();
    delete_all();
    fclose(fp_in);
    fclose(fp_out);
    return 0;
}








