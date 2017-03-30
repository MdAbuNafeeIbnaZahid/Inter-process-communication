#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
//#include <mutex.h>


#define NONE -1
#define TOTAL_STUDENT 30
#define TOTAL_APPLICATION 40

#define SIZE 9999

struct myPool
{
    pthread_mutex_t myMutex;
    long long startIdx, endIdx;
    long long stdId[SIZE], password[SIZE];
    sem_t howMuchEmpty;
    sem_t howMuchFull;
};

void *studentThreadFunction(void *arg); // stdId will be passed


int main()
{
    return 0;
}

void *studentThreadFunction(void *arg)
{
    long long a, b, c, d, e, f;
    long long stdId = *( (int*) arg );
}
