#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <limits.h>
#include <time.h>
//#include <mutex.h>


#define NONE 0
#define TOTAL_STUDENT 12
#define TOTAL_APPLICATION 15

#define SIZE 9999

#define GLOBAL_SLEEP_SEC 0

//pthread_t stdThreadAr[SIZE];
//pthread_t teacherThreadAr[SIZE];
void *threadResult;
pthread_t threadAr[SIZE];
long long globalAr[SIZE];


void *studentThreadFunction(void *arg); // stdId will be passed
void *aceThreadFunction(void *arg); // Teacher name ('A', 'C', 'E') will be passed
void *bThreadFunction(void *arg);
void *dThreadFunction(void *arg);

struct pool
{
    char name[SIZE];
    pthread_mutex_t myMutex;
    long long startIdx, endIdx;
    long long stdIdAr[SIZE], passwordAr[SIZE];
    sem_t howMuchEmpty;
    sem_t howMuchFull;

};

struct pool outstandingPool;
struct pool duplicateFilter;
struct pool quB;
struct pool quD;
struct pool bannedList;
struct pool completeList;
struct pool passwordDispatch;

void initializePool( struct pool *pool, char *name, long long siz )
{
    int res;
    strcpy(pool->name, name);
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

void pushInPool(struct pool *myPool, long long stdId, long long password, char *actorName)
{
    long long a, b, c, d, e, f;
    sem_wait( &(myPool->howMuchEmpty) );
    pthread_mutex_lock( &(myPool->myMutex) );

    (myPool->endIdx)++;
    (myPool->stdIdAr)[ (myPool->endIdx) ] = stdId;
    (myPool->passwordAr)[ (myPool->endIdx) ] = password;

//    printf("pool name = %s\n", myPool->name);
//    printf("actor = %s\n", actorName);
//    printf("element = %lld\n", stdId);
//    printf("action = push\n");
//    printf("\n\n");

    printf("pool name = %s;  "
    "actor = %s;  "
    "element = %lld;  "
    "action = push;  "
    "\n\n"
    ,myPool->name
    ,actorName
    ,stdId
    );

    pthread_mutex_unlock( &(myPool->myMutex) );
    sem_post( &(myPool->howMuchFull) );
}

long long getFrontFromPool(struct pool *myPool, char *actorName)
{
    long long a, b, c, d, e, f, ret;
    sem_wait( &(myPool->howMuchFull) );
    pthread_mutex_lock( &(myPool->myMutex) );

    //(myPool->stdIdAr)[ ++(myPool->endIdx) ] = stdId;
    ret = (myPool->stdIdAr)[ (myPool->startIdx) ];

    printf("pool name = %s;  "
    "actor = %s;  "
    "element = %lld;  "
    "action = getFrontFromPool;  "
    "\n\n"
    ,myPool->name
    ,actorName
    ,ret
    );

    pthread_mutex_unlock( &(myPool->myMutex) );

    sem_post( &(myPool->howMuchFull) );

    return ret;
}


long long popFromPool(struct pool *myPool, char *actorName)
{
    long long a, b, c, d, e, f, ret;
    sem_wait( &(myPool->howMuchFull) );
    pthread_mutex_lock( &(myPool->myMutex) );

    //(myPool->stdIdAr)[ ++(myPool->endIdx) ] = stdId;
    ret = (myPool->stdIdAr)[ (myPool->startIdx)++ ];

//    printf("pool name = %s\n", myPool->name);
//    printf("actor = %s\n", actorName);
//    printf("element = %lld\n", ret);
//    printf("action = pop\n");
//    printf("\n\n");

    printf("pool name = %s;  "
    "actor = %s;  "
    "element = %lld;  "
    "action = pop;  "
    "\n\n"
    ,myPool->name
    ,actorName
    ,ret
    );

    pthread_mutex_unlock( &(myPool->myMutex) );
    sem_post( &(myPool->howMuchEmpty) );

    return ret;
}

long long countInstance(struct pool *myPool, long long stdId)
{
    long long a, b, c, d, e, f, ret = 0;

    pthread_mutex_lock( &(myPool->myMutex) );
    for ( a = myPool->startIdx; a <= myPool->endIdx; a++ )
    {
        ret += ( (myPool->stdIdAr)[a] == stdId );
    }
    pthread_mutex_unlock( &(myPool->myMutex) );
    return ret;
}

long long getPassword(struct pool *myPool, long long stdId)
{
    long long a, b, c, d, e, f, ret = NONE;

    pthread_mutex_lock( &(myPool->myMutex) );
    for ( a = myPool->startIdx; a <= myPool->endIdx; a++ )
    {
        if ( (myPool->stdIdAr)[a] == stdId )
        {
            ret = (myPool->passwordAr)[a];
        }
    }
    pthread_mutex_unlock( &(myPool->myMutex) );
    return ret;
}

long long getPasswordAndDelete(struct pool *myPool, long long stdId)
{
    long long a, b, c, d, e,f , ret = NONE;
    pthread_mutex_lock( &(myPool->myMutex) );
    for ( a = myPool->startIdx; a <= myPool->endIdx; a++ )
    {
        if ( (myPool->stdIdAr)[a] == stdId )
        {
            ret = (myPool->passwordAr)[a];
            (myPool->stdIdAr)[a] = NONE;
            printf("Password of %lld is %lld\n\n", stdId, ret);
            break;
        }
    }
    pthread_mutex_unlock( &(myPool->myMutex) );
    return ret;
}

long long changeStdIdAndCountInstance(struct pool *myPool, long long oldStdId, long long newStdId)
{
    long long a, b, c, d, e, f, ret = 0;
    pthread_mutex_lock( &(myPool->myMutex) );
    for ( a = myPool->startIdx; a <= myPool->endIdx; a++ )
    {
        if ( (myPool->stdIdAr)[a] == oldStdId )
        {
            (myPool->stdIdAr)[a] = newStdId;
            ret++;
        }
    }
    pthread_mutex_unlock( &(myPool->myMutex) );
    return ret;
}

long long generatePassword(long long stdId)
{
    long long ret = ( 1+clock()%71 ) * 100 + stdId;
//    printf("In generate password, stdId = %lld, ret = %lld\n\n", stdId, ret);
    return ret;
}

void initDS()
{
    initializePool( &outstandingPool, "outstandingPool", 10 );
//    printf("Safely initialized outstandingPool\n");
    initializePool( &duplicateFilter, "duplicateFilter", INT_MAX );
//    printf("Safely initialized duplicate pool \n");
    initializePool(&quB, "quB", 1);
//    printf("Safely initialized queue for B \n");
    initializePool(&bannedList, "bannedList", INT_MAX);
    initializePool(&completeList, "completeList", INT_MAX);
//    printf("Safely initialized banned list \n");
    initializePool(&quD, "quD", 1);
//    printf("Safely initialized queue for D \n");
    initializePool(&passwordDispatch, "password dispatch", 1);
}

void createThreads()
{
    long long a, b, c, d, e, f, idx = 0;

    pthread_create( threadAr+(idx++), NULL, aceThreadFunction, (void*)(globalAr+'A') );
    pthread_create( threadAr+(idx++), NULL, aceThreadFunction, (void*)(globalAr+'C') );
    pthread_create( threadAr+(idx++), NULL, aceThreadFunction, (void*)(globalAr+'E') );

    //creating B
    pthread_create( threadAr+(idx++), NULL, bThreadFunction, (void*)(globalAr+'B') );

    //creting D
    pthread_create( threadAr+(idx++), NULL, dThreadFunction, (void*)(globalAr+'D') );

    for ( a = 1; a <= TOTAL_APPLICATION; a++)
    {
        sleep(GLOBAL_SLEEP_SEC);
        pthread_create( threadAr+(idx++), NULL, studentThreadFunction, (void*)(globalAr+ (1+a%TOTAL_STUDENT) ) );
        sleep(GLOBAL_SLEEP_SEC);
    }
    //idx = TOTAL_APPLICATION + 1;

    // creating ACE


}

void joinThreads()
{
    long long a, b, c, d, e, f;
    for ( a = 1; a <= TOTAL_APPLICATION+5; a++)
    {
        pthread_join( threadAr[a], &threadResult );
    }
}

void init()
{
    long long a, b, c, d, e, f;
    for ( a = 0; a < SIZE; a++)
    {
        globalAr[a] = a;
    }
    initDS();
    createThreads();
    joinThreads();
}




int main()
{
    init();
    return 0;
}

void *studentThreadFunction(void *arg)
{
    long long a, b, c, d, e, f, passwordGot = NONE;
    long long stdId = *( (long long*) arg );
    char stdIdStr[SIZE];
    sprintf(stdIdStr, "%lld", stdId);
    sleep(GLOBAL_SLEEP_SEC);

    //printf("%lld trying to put application in outstanding pool\n\n", stdId);
    pushInPool( &outstandingPool, stdId, NONE, stdIdStr );
    //printf("%lld successfully submitted application in outstanding pool\n\n", stdId);
    sleep(GLOBAL_SLEEP_SEC);

    //printf("%lld trying to put stdid in  quB\n\n", stdId);
    pushInPool( &quB, stdId, NONE, stdIdStr );
    //printf("%lld successfully put std id in qu B\n\n", stdId);
    sleep(GLOBAL_SLEEP_SEC);

    while(passwordGot == NONE)
    {
        //printf("%lld trying to put stdid in  quD\n\n", stdId);
        pushInPool( &quD, stdId, NONE, stdIdStr );
        //printf("%lld successfully put std id in qu B\n\n", stdId);
        sleep(GLOBAL_SLEEP_SEC);

        passwordGot = popFromPool(&passwordDispatch, stdIdStr);
        sleep(GLOBAL_SLEEP_SEC);
    }
    printf("Std %lld has finally got password %lld\n\n", stdId, passwordGot);
}

void *aceThreadFunction(void *arg)
{
    long long a, b, c, d, e, f, poppedStd;
    char teacherName = *( (long long*) arg ) ;
    char teacherNameStr[SIZE];
    sprintf(teacherNameStr, "%c", teacherName);
    sleep(GLOBAL_SLEEP_SEC);
    printf(" Teacher %c  started working \n\n", teacherName);
    sleep(GLOBAL_SLEEP_SEC);
    while(1)
    {
        //printf("Teacher %c is trying to pop a student from outstanding pool\n\n", teacherName);
        sleep(GLOBAL_SLEEP_SEC);
        poppedStd = popFromPool( &outstandingPool, teacherNameStr );
        //printf("Teacher %c has popped std %lld from outstanding pool \n\n", teacherName, poppedStd);
        sleep(GLOBAL_SLEEP_SEC);

        pushInPool(&duplicateFilter, poppedStd, NONE, teacherNameStr);
        sleep(GLOBAL_SLEEP_SEC);
    }
}

void *bThreadFunction(void *arg)
{
    long long a, b, c, d, e, f, frontStdId, isBanned, howManyInDuplicateFilter;
    char teacherName = *( (long long*) arg ) ;
    char teacherNameStr[SIZE];
    sprintf(teacherNameStr, "%c", teacherName);
    sleep(GLOBAL_SLEEP_SEC);
    printf(" Teacher %c  started working \n\n", teacherName);
    sleep(GLOBAL_SLEEP_SEC);
    while(1)
    {
        //printf("Teacher %c is trying to pop a student from qu B\n\n", teacherName);
        sleep(GLOBAL_SLEEP_SEC);
        frontStdId = getFrontFromPool( &quB, teacherNameStr );
//        printf("Teacher %c has popped std %lld from qu B\n\n", teacherName, poppedStdId);
        sleep(GLOBAL_SLEEP_SEC);

        isBanned = countInstance( &bannedList, frontStdId );

        sleep(GLOBAL_SLEEP_SEC);
        if ( isBanned )
        {
            printf("std %lld is already banned \n\n", frontStdId);
            popFromPool(&quB, "B");
            continue;
        }

        howManyInDuplicateFilter = changeStdIdAndCountInstance(&duplicateFilter, frontStdId, NONE);
        if ( howManyInDuplicateFilter == 0 )
        {
            sem_init( &(duplicateFilter.howMuchFull), 0, 0);
        }

        while( howManyInDuplicateFilter == 0 )
        {
            sem_wait( &(duplicateFilter.howMuchFull) );
            howManyInDuplicateFilter =
                changeStdIdAndCountInstance(&duplicateFilter, frontStdId, NONE);
            sleep(GLOBAL_SLEEP_SEC);
        }

        printf("std %lld has appeared %lld times in duplicate filter\n\n",
            frontStdId, howManyInDuplicateFilter);
        sleep(GLOBAL_SLEEP_SEC);
        if ( howManyInDuplicateFilter > 1 )
        {
            pushInPool(&bannedList, frontStdId, NONE, teacherNameStr);
            //printf("B has banned std %lld\n\n", poppedStdId);
            sleep(GLOBAL_SLEEP_SEC);
            popFromPool(&quB, "B");
            continue;
        }


        if ( howManyInDuplicateFilter == 1 )
        {
            pushInPool( &completeList, frontStdId, generatePassword( frontStdId ), teacherNameStr );
            //printf("B forwarded std %lld to get password\n\n", poppedStdId);
            sleep(GLOBAL_SLEEP_SEC);
            popFromPool(&quB, "B");
            continue;
        }

//        printf("Perhaps application of std %lld is not yet approved by A,C,E yet\n\n", poppedStdId);
//        sem_init( &(duplicateFilter->howMuchFull), 0, 0 );
//        sleep(5);
//        while(1)
//        {
//            sem_wait( &(duplicateFilter->howMuchFull) );
//            howManyInDuplicateFilter =
//                changeStdIdAndCountInstance(&duplicateFilter, poppedStdId, NONE);
//            if ( howManyInDuplicateFilter == 0 )
//            {
//                continue;
//            }
//            else
//            {
//                break;
//            }
//            sleep(5);
//        }

//        if ( isBanned || (howManyInDuplicateFilter > 1) )
//        {
//            pushInPool(&bannedList, poppedStdId, NONE);
//            printf("B rejected std %lld\n\n", poppedStdId);
//        }
//        else
//        {
//            pushInPool(&completeList, poppedStdId, genPass() );
//            printf("B forwarded std %lld to get password\n\n", poppedStdId);
//        }
//        sleep(5);
//
//        changeStdIdAndCountInstance(&duplicateFilter, poppedStdId, NONE);
    }
}

void *dThreadFunction(void *arg)
{
    long long a, b, c, d, e, f, poppedStdId, retrievedPassword, frontStdId;
    printf("Teacher D started working \n\n");
    sleep(GLOBAL_SLEEP_SEC);
    while(1)
    {
        frontStdId = getFrontFromPool(&quD, "D");
        sleep(GLOBAL_SLEEP_SEC);
        retrievedPassword = getPasswordAndDelete(&completeList, frontStdId);
        //printf("Teacher D has checked complete list for std %lld and got password %lld\n\n", poppedStdId,
            //retrievedPassword);
        sleep(GLOBAL_SLEEP_SEC);
        pushInPool(&passwordDispatch, retrievedPassword, retrievedPassword, "D");
        // Now make sure that std has received answer from passwordDispatch
        // for this reason I will push a dummy value and pop it from pool
        pushInPool(&passwordDispatch, NONE, NONE, "D");
        popFromPool(&passwordDispatch, "D");
        // it is sure student has received password
        popFromPool(&quD, "D"); // now we can safely pop from quD so that another student may
        // push in quD
        sleep(GLOBAL_SLEEP_SEC);
    }
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
