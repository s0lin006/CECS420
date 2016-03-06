struct thread_argument
{
    char arg1[MAXLINESIZE];
    char arg2[25];
}t_arg;

void *print_message_function(void *arguments)
{
    struct thread_argument *targ = (struct thread_argument *)arguments;
    printf(">>>%s\n", targ->arg1);
    printf(">>>%s\n", targ->arg2);
    
    //free(targ);
    pthread_exit(NULL);
}

int main()
{
    
    pid_t n;
    pthread_t thread1, thread2;
    const char *message1 = "Thread 1";
    const char *message2 = "Thread 2";
    int iret1, iret2;
    
    struct thread_argument *args = malloc(sizeof(*args));
    args->arg1 = 5;
    args->arg2 = 7;
    
    n = fork(); //fork a child
    if(n < 0)
    {
        fprintf(stderr, "Fork Failed\n");
        exit(-1);
    }
    else if(n == 0)
    {
        iret1 = pthread_create(&thread1, NULL, print_message_function, (void*)args);
        if(iret1)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
            exit(EXIT_FAILURE);
        }
    
        pthread_join (thread1, NULL);
    }
    else
    {
        fprintf(stdout, "Parent\n");
        wait(NULL);
        fprintf(stdout, "Child Complete\n");
        exit(0);
    }
    
    //pthread_exit(NULL);
    
    return 0;
}
