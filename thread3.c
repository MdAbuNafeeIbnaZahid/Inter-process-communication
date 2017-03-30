#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define WORK_SIZE 1024

char workArea[WORK_SIZE];

void *threadFunction(void *arg);

sem_t binSem;

int main()
{
    int res;
    pthread_t aThread;
    void *thrResult;

    res = sem_init(&binSem, 0, 0);
    if ( res != 0 )
    {
        perror("Semaphore Initialization failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&aThread, NULL, threadFunction, NULL);
    if ( res != 0 )
    {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Input some text. Enter 'end' to finish\n");
    while( strncmp("end", workArea, 3) != 0 )
    {
        fgets(workArea, WORK_SIZE, stdin);
        sem_post( &binSem );
    }


    printf("\nWaiting for the thread to finish\n");
    res = pthread_join(aThread, &thrResult);
    if ( res != 0 )
    {
        perror("Thread join failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Thread joined\n");
    sem_destroy(&binSem);
    exit(EXIT_SUCCESS);
}


void *threadFunction(void *arg)  // main is sending NULL as arg
{
    while(1)
    {
        sem_wait(&binSem);
        if ( strncmp("end", workArea, 3) == 0 )
        {
            break;
        }
        printf("%d chars\n", (int)(strlen(workArea)-1) );
    }

}
