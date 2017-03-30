#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define SIZE 3

void *thread_function(void *arg);;

char message[] = "Hello world";

int intAr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int globalCnt;

int main()
{
    int res;
    pthread_t a_thread_ar[SIZE];
    void *thread_result[SIZE];

    for (int a = 0; a < SIZE; a++)
    {
        printf("In loop ...  a= %d \n", a);
        res = pthread_create(a_thread_ar+a, NULL, thread_function, (void*) (intAr+a) );
        if ( res != 0 )
        {
            printf("Thread %d execution failed \n", a);
            perror("Thread execution failed \n");
            exit(EXIT_FAILURE);
        }


//
    }

//    printf("Waiting for the threads to   finish...\n");
    for (int a = 0; a < SIZE; a++)
    {

        res = pthread_join(a_thread_ar[a], thread_result+a);
        if ( res != 0 )
        {
            printf("Thread %d join failed \n", a);
            perror("Thread join failed \n");
            exit(EXIT_FAILURE);
        }
    }

//    printf(" Thread joined ,,, it returned %s \n ", (char*)thread_result );
//    printf("Message is now %s \n", message);

//    for ( int a = 0; a < SIZE ; a++ )
//    {
//        printf("Thread %d joined...  it returned %d \n", a, *( (int*)thread_result[a] ) );
//        printf(" intAr[%d] = %d \n", a, intAr[a]);
//    }

    exit(EXIT_SUCCESS);
}

void *thread_function(void *arg)
{
    int threadNo = *((int*)arg);
    printf("Thread function is running. Argument is %d \n", *((int*)arg) );
    sleep(3);

    //*((int*)arg) = *((int*)arg) * 100;
    //strcpy(message, "Bye");
    //char ret[] = "Ret from child thread";
    int *ret = (int *)malloc( sizeof(int) );
    *ret = *((int*)arg) + 9;
    int loop = 0;
    while( globalCnt < SIZE * SIZE )
    {
        //printf("In thread %d .....  loop = %d \n", *((int*)arg), loop++);
        sleep(2);
        if ( globalCnt % ( SIZE ) == threadNo )
        {
            printf("globalCnt = %d , threadNo = %d \n", globalCnt, threadNo);
            globalCnt++;

        }
    }
    pthread_exit( (void*) ret );
}
