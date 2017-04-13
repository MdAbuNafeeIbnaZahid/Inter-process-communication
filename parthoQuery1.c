#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <limits.h>
#include <time.h>
//#include <mutex.h>

void *a;
void * tester(void * arg){
    printf("hello guys i'm %d now i will sleep\n",*((int *)arg));
    sleep(0);
    printf("hello guys i wake up %d\n",*((int *)arg));

}

int main()
{
    printf("Start of main \n\n");
    int i, ar[99];
    for(i=0;i<10;i++)
    {
        ar[i]=i+1;
        pthread_t thread1;
        pthread_create(&thread1,NULL,tester,(void*)(&ar[i] ));
        //sleep(500);
        pthread_join(thread1, &a );
    }
}
