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
#include <math.h>

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

#define true 1
#define false 0
#define bool int

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
}*head, *last, *head2, *last2, *head3, *last3, *head4, *last4;


int get_psn(int lbn, int request)
{
    int x = 8 * lbn;
    x = x + request;
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

double get_seek_distance_scan(int prev_cylinder, int curr_cylinder)
{
    double x = 0;
    x = curr_cylinder - prev_cylinder;
    if(x < 0)
    {
        x = (CYLINDERS - prev_cylinder) + (CYLINDERS - curr_cylinder) - 2;
    }
    return x;
}

double get_seek_distance_clook(int prev_cylinder, int curr_cylinder)
{
    double x = 0;
    x = curr_cylinder - prev_cylinder;
    if(x < 0)
    {
        x = x * -1;
    }
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

void find_head2()
{
    if(head2 == NULL)
    {
        printf(">>>List is empty\n");
    }
    else
    {
        while(head2->prev)
        {
            head2 = head2->prev;
        }
    }
}

void find_head3()
{
    if(head3 == NULL)
    {
        printf(">>>List is empty\n");
    }
    else
    {
        while(head3->prev)
        {
            head3 = head3->prev;
        }
    }
}

void find_head4()
{
    if(head4 == NULL)
    {
        printf(">>>List is empty\n");
    }
    else
    {
        while(head4->prev)
        {
            head4 = head4->prev;
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

void sort_by_cylinder()
{
    struct node *temp1, *temp2;
    temp1 = head->next;
    
    while((temp1->prev != NULL) && (temp1 != NULL))
    {
        temp2 = temp1;

        while((temp2->prev != NULL)&&((temp2->cylinder) < (temp2->prev->cylinder)))
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
    
    while(temp != NULL)
    {
        printf("%lf %lf %lf %d %d %d %lf %d\n", temp->arrival_time, temp->finish_time, temp->waiting_time, temp->physical_sector_number, temp->cylinder, temp->surface, temp->sector_offset, temp->seek_distance);
        
        temp = temp->next;
    }
}

void display2()
{
    struct node *temp;
    temp = head2;
    
    while(temp != NULL)
    {
        printf("%lf %lf %lf %d %d %d %lf %d\n", temp->arrival_time, temp->finish_time, temp->waiting_time, temp->physical_sector_number, temp->cylinder, temp->surface, temp->sector_offset, temp->seek_distance);
        
        temp = temp->next;
    }
}

void display3()
{
    struct node *temp;
    temp = head3;
    
    while(temp != NULL)
    {
        printf("%lf %lf %lf %d %d %d %lf %d\n", temp->arrival_time, temp->finish_time, temp->waiting_time, temp->physical_sector_number, temp->cylinder, temp->surface, temp->sector_offset, temp->seek_distance);
        
        temp = temp->next;
    }
}

void display4()
{
    struct node *temp;
    temp = head4;
    
    while(temp != NULL)
    {
        printf("%lf %lf %lf %d %d %d %lf %d\n", temp->arrival_time, temp->finish_time, temp->waiting_time, temp->physical_sector_number, temp->cylinder, temp->surface, temp->sector_offset, temp->seek_distance);
        
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

void print_to_file4(FILE *fp_out)
{
    struct node *temp;
    temp = head4;
    
    
    while(temp != NULL)
    {
        fprintf(fp_out, "%lf %lf %lf %d %d %d %lf %d\n", temp->arrival_time, temp->finish_time, temp->waiting_time, temp->physical_sector_number, temp->cylinder, temp->surface, temp->sector_offset, temp->seek_distance);
        
        temp = temp->next;
    }
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

double current_offset = 0;
double target_offset = 0;

double get_finish_scan(double arrival, double waiting, int prev_cylinder, int curr_cylinder, int request_size, double prev_sector_offset, double offset)
{
    double x, service, seek, rotational, transfer, offset_dist;
    int d;
    current_offset = 0;
    target_offset = offset - request_size;
    
    //SEEK TIME
    d = curr_cylinder - prev_cylinder;
    if(d < 0)
    {
        seek = (CYLINDERS - prev_cylinder) + (CYLINDERS - curr_cylinder) - 2;
        seek = (0.000028 * seek) + 2;
    }
    else if(d == 0)
    {
        seek = 0;
    }
    else
    {
        seek = (0.000028 * d) + 2;
    }
    //END SEEK TIME
    //TRANSFER TIME
    transfer = (512 * 1000 * request_size);
    transfer = transfer / (134217728);
    //END TRANSFER TIME
    //ROTATIONAL LATENCY
    current_offset = seek/0.03;
    current_offset = current_offset + prev_sector_offset;
    while(current_offset > 200)
    {
        current_offset = current_offset - 200;
    }
    
    target_offset = offset - request_size;
    if(current_offset < target_offset)
    {
        offset_dist = target_offset - current_offset;
    }
    else if(current_offset > target_offset)
    {
        offset_dist = 200 - current_offset;
        offset_dist = offset_dist + target_offset;
    }
    else
    {
        //DO NOTHING
    }
    rotational = 0.03 * offset_dist;
    //END ROTATIONAL LATENCY
    service = seek + rotational + transfer;
    service = service / 1000; //millisecond to second
    
    x = arrival + waiting + service;
    return x;
}

double get_finish_clook(double arrival, double waiting, int prev_cylinder, int curr_cylinder, int request_size, double prev_sector_offset, double offset)
{
    double x, service, seek, rotational, transfer, offset_dist;
    int d;
    current_offset = 0;
    target_offset = offset - request_size;
    
    //SEEK TIME
    d = curr_cylinder - prev_cylinder;
    if(d < 0)
    {
        d = d * -1;
        seek = (0.000028 * d) + 2;
    }
    else if(d == 0)
    {
        seek = 0;
    }
    else
    {
        seek = (0.000028 * d) + 2;
    }
    //END SEEK TIME
    //TRANSFER TIME
    transfer = (512 * 1000 * request_size);
    transfer = transfer / (134217728);
    //END TRANSFER TIME
    //ROTATIONAL LATENCY
    current_offset = seek/0.03;
    current_offset = current_offset + prev_sector_offset;
    while(current_offset > 200)
    {
        current_offset = current_offset - 200;
    }
    
    target_offset = offset - request_size;
    if(current_offset < target_offset)
    {
        offset_dist = target_offset - current_offset;
    }
    else if(current_offset > target_offset)
    {
        offset_dist = 200 - current_offset;
        offset_dist = offset_dist + target_offset;
    }
    else
    {
        //DO NOTHING
    }
    rotational = 0.03 * offset_dist;
    //END ROTATIONAL LATENCY
    service = seek + rotational + transfer;
    service = service / 1000; //millisecond to second
    
    x = arrival + waiting + service;
    return x;
}

void time_info(bool scan)
{
    find_head();
    struct node *temp;
    temp = head;
    
    while(temp != NULL)
    {
        if(temp == head)
        {
            temp->waiting_time = 0;
            if(scan == 1)
            {
                temp->finish_time = get_finish_scan(temp->arrival_time, 0, 0, temp->cylinder, temp->request_size, 0, temp->sector_offset);
            }
            else
            {
                temp->finish_time = get_finish_clook(temp->arrival_time, 0, 0, temp->cylinder, temp->request_size, 0, temp->sector_offset);
            }
        }
        else
        {
            temp->waiting_time = get_waiting(temp->arrival_time, temp->prev->finish_time);
            if(scan == 1)
            {
                temp->finish_time = get_finish_scan(temp->arrival_time, temp->waiting_time, temp->prev->cylinder, temp->cylinder, temp->request_size, temp->prev->sector_offset, temp->sector_offset);
            }
            else
            {
                temp->finish_time = get_finish_clook(temp->arrival_time, temp->waiting_time, temp->prev->cylinder, temp->cylinder, temp->request_size, temp->prev->sector_offset, temp->sector_offset);
            }
        }
        temp = temp->next;
    }
}

void info(bool scan)
{
    find_head();
    struct node *temp;
    temp = head;
    
    while(temp != NULL)
    {
        if(temp == head)
        {
            temp->physical_sector_number = get_psn(temp->logical_block_number, temp->request_size);
            temp->cylinder = get_cylinder(temp->physical_sector_number);
            temp->sector_offset = get_sector_offset(temp->physical_sector_number);
            temp->surface = get_surface(temp->physical_sector_number);
            if(scan == 1)
            {
                temp->seek_distance = get_seek_distance_scan(0, temp->cylinder);
            }
            else
            {
                temp->seek_distance = get_seek_distance_clook(0, temp->cylinder);
            }
        }
        else
        {
            temp->physical_sector_number = get_psn(temp->logical_block_number, temp->request_size);
            temp->cylinder = get_cylinder(temp->physical_sector_number);
            temp->sector_offset = get_sector_offset(temp->physical_sector_number);
            temp->surface = get_surface(temp->physical_sector_number);
            if(scan == 1)
            {
                temp->seek_distance = get_seek_distance_scan(temp->prev->cylinder, temp->cylinder);
            }
            else
            {
                temp->seek_distance = get_seek_distance_clook(temp->prev->cylinder, temp->cylinder);
            }
        }
        temp = temp->next;
    }
}

void time_info4(bool scan)
{
    find_head4();
    struct node *temp;
    temp = head4;
    
    while(temp != NULL)
    {
        if(temp == head4)
        {
            temp->waiting_time = 0;
            if(scan == 1)
            {
                temp->finish_time = get_finish_scan(temp->arrival_time, 0, 0, temp->cylinder, temp->request_size, 0, temp->sector_offset);
            }
            else
            {
                temp->finish_time = get_finish_clook(temp->arrival_time, 0, 0, temp->cylinder, temp->request_size, 0, temp->sector_offset);
            }
        }
        else
        {
            temp->waiting_time = get_waiting(temp->arrival_time, temp->prev->finish_time);
            if(scan == 1)
            {
                temp->finish_time = get_finish_scan(temp->arrival_time, temp->waiting_time, temp->prev->cylinder, temp->cylinder, temp->request_size, temp->prev->sector_offset, temp->sector_offset);
            }
            else
            {
                temp->finish_time = get_finish_clook(temp->arrival_time, temp->waiting_time, temp->prev->cylinder, temp->cylinder, temp->request_size, temp->prev->sector_offset, temp->sector_offset);
            }
        }
        temp = temp->next;
    }
}

void info4(bool scan)
{
    find_head4();
    struct node *temp;
    temp = head4;
    
    while(temp != NULL)
    {
        if(temp == head4)
        {
            temp->physical_sector_number = get_psn(temp->logical_block_number, temp->request_size);
            temp->cylinder = get_cylinder(temp->physical_sector_number);
            temp->sector_offset = get_sector_offset(temp->physical_sector_number);
            temp->surface = get_surface(temp->physical_sector_number);
            if(scan == 1)
            {
                temp->seek_distance = get_seek_distance_scan(0, temp->cylinder);
            }
            else
            {
                temp->seek_distance = get_seek_distance_clook(0, temp->cylinder);
            }
        }
        else
        {
            temp->physical_sector_number = get_psn(temp->logical_block_number, temp->request_size);
            temp->cylinder = get_cylinder(temp->physical_sector_number);
            temp->sector_offset = get_sector_offset(temp->physical_sector_number);
            temp->surface = get_surface(temp->physical_sector_number);
            if(scan == 1)
            {
                temp->seek_distance = get_seek_distance_scan(temp->prev->cylinder, temp->cylinder);
            }
            else
            {
                temp->seek_distance = get_seek_distance_clook(temp->prev->cylinder, temp->cylinder);
            }
        }
        temp = temp->next;
    }
}

void insert_to_keep(double larrival_time, double lfinish_time, int llogical_block_number, int lrequest_size)
{
    struct node *var, *temp;
    var = (struct node*)malloc(sizeof(struct node));
        var->arrival_time = larrival_time;
        var->finish_time = lfinish_time;
        var->logical_block_number = llogical_block_number;
        var->request_size = lrequest_size;
    if(head2 == NULL)
    {
        head2 = var;
        head2->prev = NULL;
        head2->next = NULL;
        last2 = head2;
    }
    else
    {
        last2 = head2;
        while(last2 != NULL)
        {
            temp = last2;
            last2 = last2->next;
        }
        last2 = var;
        temp->next = last2;
        last2->prev = temp;
        last2->next = NULL;
    }
}

void insert_to_skip(double larrival_time, double lfinish_time, int llogical_block_number, int lrequest_size)
{
    struct node *var, *temp;
    var = (struct node*)malloc(sizeof(struct node));
        var->arrival_time = larrival_time;
        var->finish_time = lfinish_time;
        var->logical_block_number = llogical_block_number;
        var->request_size = lrequest_size;
    if(head3 == NULL)
    {
        head3 = var;
        head3->prev = NULL;
        head3->next = NULL;
        last3 = head3;
    }
    else
    {
        last3 = head3;
        while(last3 != NULL)
        {
            temp = last3;
            last3 = last3->next;
        }
        last3 = var;
        temp->next = last3;
        last3->prev = temp;
        last3->next = NULL;
    }
}

double curr_finish = 0;

void SCAN()
{
    struct node *temp;
    temp = head;
    while(temp != NULL)
    {
        if(temp == head)
        {
            insert_to_keep(temp->arrival_time, temp->finish_time, temp->logical_block_number, temp->request_size);
            curr_finish = temp->finish_time;
        }
        else
        {
            if(temp->arrival_time <= curr_finish)
            {
                insert_to_keep(temp->arrival_time, temp->finish_time, temp->logical_block_number, temp->request_size);
                curr_finish = temp->finish_time;
            }
            else
            {
                insert_to_skip(temp->arrival_time, temp->finish_time, temp->logical_block_number, temp->request_size);
            }
        }
        temp = temp->next;
    }
}


void insert_to_final()
{
    struct node *var, *temp, *t;
    find_head2();
    t= head2;
    while(t !=NULL)
    {
        var = (struct node*)malloc(sizeof(struct node));
            var->arrival_time = t->arrival_time;
            var->finish_time = t->finish_time;
            var->logical_block_number = t->logical_block_number;
            var->request_size = t->request_size;
        if(head4 == NULL)
        {
            head4 = var;
            head4->prev = NULL;
            head4->next = NULL;
            last4 = head4;
        }
        else
        {
            last4 = head4;
            while(last4 != NULL)
            {
                temp = last4;
                last4 = last4->next;
            }
            last4 = var;
            temp->next = last4;
            last4->prev = temp;
            last4->next = NULL;
        }
        t = t->next;
    }
    find_head3();
    t= head3;
    while(t !=NULL)
    {
        var = (struct node*)malloc(sizeof(struct node));
            var->arrival_time = t->arrival_time;
            var->finish_time = t->finish_time;
            var->logical_block_number = t->logical_block_number;
            var->request_size = t->request_size;
        if(head4 == NULL)
        {
            head4 = var;
            head4->prev = NULL;
            head4->next = NULL;
            last4 = head4;
        }
        else
        {
            last4 = head4;
            while(last4 != NULL)
            {
                temp = last4;
                last4 = last4->next;
            }
            last4 = var;
            temp->next = last4;
            last4->prev = temp;
            last4->next = NULL;
        }
        t = t->next;
    }
}
int main(int argc, char *argv[])
{
    FILE *fp_in;
    FILE *fp_out;
    fp_in = fopen(argv[1], "r");
    fp_out = fopen(argv[2], "w");
    
    int scan = 0;
    if(strcmp("SCAN", argv[3]) == 0)
    {
TRACE
        scan = 1;
    }
    else
    {
TRACE
        scan = 0;
    }
    
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
    
    //find_head();
    //time_info();
    
    //find_head();
    //insert_sort();
    find_head();
    info(scan);
    
    find_head();
    time_info(scan);
    
    find_head();
    sort_by_cylinder();
    
    
    
    //find_head();
    //print_to_file(fp_out);
    //display();
    find_head();
    SCAN();
    
    find_head4();
    insert_to_final();
    //display4();
    
    find_head4();
    info4(scan);
    
    find_head4();
    time_info4(scan);
    
    find_head4();
    print_to_file4(fp_out);
    //display4();
    
    find_head();
    delete_all();
    fclose(fp_in);
    fclose(fp_out);
    return 0;
}








