/*
*   CECS 420
*   Project 1 - uoflinsort.c
*   Name: Shan Lin
*   Due: 01.29.2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student
{
    int id;
    char *first;
    char *last;
    char *department;
    float gpa;
    struct student *prev;
    struct student *next;
}*head, *last;

void insert(int lid, char *lfirst, char *llast, char *ldepartment, float lgpa)
{
    struct student *var, *temp;
    var = (struct student*)malloc(sizeof(struct student));
        var->id = lid;
        var->first = lfirst;
        var->last = llast;
        var->department = ldepartment;
        var->gpa = lgpa;
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
        while(last!=NULL)
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
    struct student *temp;
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
    while(head != NULL)
    {
        delete_node();
    }
}

void swap(struct student *left, struct student *right)
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
void remove_dup()
{
    struct student *temp;
    temp = head;
    
    if(head == NULL)
    {
        printf("List iss empty\n");
    }
    else
    {
        while (temp!= NULL && temp->next != NULL)
        {
            if(temp->id == temp->next->id)
            {
                if(temp->prev == NULL || temp->next == NULL)
                {
                    //do something
                }
                else
                {
                    temp->prev->next = temp->next;
                    temp->next->prev = temp->prev;
                
                    temp->prev = NULL;
                    temp->next = NULL;
                    free(temp);
                }
            }
            temp = temp->next;
        }
    }
}
void insert_sort()
{
    struct student *temp1, *temp2;
    temp1 = head->next;
    
    //tracking purpose - delete once project is complete
    //int  i = 0;
    
    while((temp1->prev != NULL) && (temp1 != NULL))
    {
        temp2 = temp1;

        while((temp2->prev != NULL)&&((temp2->id) < (temp2->prev->id)))
        {
            printf("Swapping: %d and %d\n", temp2->prev->id, temp2->id);
            swap(temp2->prev, temp2);
            //printf(">>>>Swapped%d\n", temp2->id);
        }
        if(temp1->next == NULL)
        {
            break;
            //printf("BORKE");
        }
        else
        {
            temp1 = temp1->next;
            //printf("temp1: %d\n", temp1->id);
        }
        //i = i + 1;
        //printf("%d\n", i);
    }
}

void display()
{
    struct student *temp;
    temp = head;
    if(temp == NULL)
    {
        printf("list is empty\n");
    }
    while(temp != NULL)
    {
        //printf("-> %d\n", temp->id);
        temp = temp->next;
    }
}

void print_to_file(FILE *fp_out)
{
    struct student *temp;
    temp = head;
    if(temp == NULL)
    {
        fprintf(fp_out, "list is empty\n");
    }
    while(temp != NULL)
    {
        fprintf(fp_out, "%d,%s,%s,%s,%.2f\n",temp->id, temp->first, temp->last, temp->department, temp->gpa);
        temp = temp->next;
    }
}

int main(int argc, char *argv[])
{
    //file in.txt and out.txt for reading and writing
    FILE *fp_in;
    FILE *fp_out;
    fp_in = fopen(argv[1], "r");
    fp_out = fopen(argv[2], "w");
    
    //local variables
    int lid;
    char *lfirst;
    char *llast;
    char *ldepartment;
    float lgpa;
    
    //read in data from in file
    if (fp_in ==NULL || fp_out == NULL)
    {
        printf("\nFile cannot be read\n");
    }
    else
    {
        //printf("\nid,fname,lname,dept,gpa\n");//TakeThisOutOnceDone
        fscanf(fp_in, "%d%ms%ms%ms%f", &lid, &lfirst, &llast, &ldepartment, &lgpa);
        
        while(!feof(fp_in))
        {
            insert(lid, lfirst, llast, ldepartment, lgpa);
            fscanf(fp_in, "%d%ms%ms%ms%f", &lid, &lfirst, &llast, &ldepartment, &lgpa);
            //printf("inserting: %d\n", lid);
        }
    }//end else
    
    //sorting
    find_head();
    insert_sort();
    
    //checking for duplicates
    find_head();
    remove_dup();
    
    //print to out file
    find_head();
    print_to_file(fp_out);
    
    //free-ing list
    find_head();
    delete_all();
    
    //closing in.txt and out.txt
    fclose(fp_in);
    fclose(fp_out);
    
    return 0;
}
