#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <limits.h>
//#include <mutex.h>


#define NONE -1
#define TOTAL_STUDENT 30
#define TOTAL_APPLICATION 40

#define SIZE 9999

pthread_t stdThreadAr[SIZE];

struct pool
{
    pthread_mutex_t myMutex;
    long long startIdx, endIdx;
    long long stdIdAr[SIZE], passwordAr[SIZE];
    sem_t howMuchEmpty;
    sem_t howMuchFull;

};
struct pool outstandingPool;
struct pool duplicateFilter;

void initializePool( struct pool *pool, long long siz )
{
    int res;
    res = pthread_mutex_init( &(pool->myMutex), NULL );
    if (res != 0)
    {
        printf("Failed to initialize mutex");
        exit(0);
    }
    pool->startIdx = 1;
    pool->endIdx = 0;
    res = sem_init( &(pool->howMuchEmpty), 0, siz  );
    if (res != 0)
    {
        printf("Failed to initialize semaphore");
        exit(0);
    }
    res = sem_init( &(pool->howMuchFull), 0, 0  );
    if (res != 0)
    {
        printf("Failed to initialize semaphore");
        exit(0);
    }
}

void pushInPool(struct pool *myPool, long long stdId)
{
    long long a, b, c, d, e, f;
    sem_wait( &(myPool->howMuchEmpty) );
    pthread_mutex_lock( &(myPool->myMutex) );

    (myPool->stdIdAr)[ ++(myPool->endIdx) ] = stdId;


    pthread_mutex_unlock( &(myPool->myMutex) );
    sem_post( &(myPool->howMuchFull) );
}


void *studentThreadFunction(void *arg); // stdId will be passed
void *teacherThreadFunction(void *arg); // Teacher name (A, C, E) will be passed


int main()
{
    initializePool( &outstandingPool, 10 );
    printf("Safely initialized outstandingPool\n");
    initializePool( &duplicateFilter, INT_MAX );
    printf("Safely initialized duplicate pool \n");
    return 0;
}

void *studentThreadFunction(void *arg)
{
    long long a, b, c, d, e, f;
    long long stdId = *( (int*) arg );
}


// Target is not to use
// It is obsolete
void initializeOustandingPool()
{
    int res;
    res = pthread_mutex_init( &(outstandingPool.myMutex), NULL );
    if (res != 0)
    {
        printf("Failed to initialize mutex");
        exit(0);
    }
    outstandingPool.startIdx = 1;
    outstandingPool.endIdx = 0;
    res = sem_init( &(outstandingPool.howMuchEmpty), 0, 10  );
    if (res != 0)
    {
        printf("Failed to initialize semaphore");
        exit(0);
    }
    res = sem_init( &(outstandingPool.howMuchFull), 0, 0  );
    if (res != 0)
    {
        printf("Failed to initialize semaphore");
        exit(0);
    }

}
