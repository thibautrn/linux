#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int count= 0;

pthread_mutex_t mutex;



#define LOCK 1
volatile int isLocked = 0;

void unlock(){
   asm("movl    $0, %%eax;"         /* eax = 0 */
      "xchgl   %%eax, %0;"          /* Exchange register eax and isLocked */
      :"=r"(isLocked));             /* outputs */
    printf("unlock est %d\n", isLocked);
}

int testAndSet(volatile int *lockPtr){
    volatile int oldValue;
    // oldValue = *lockPtr
    asm("movl %1, %%eax;"
        "movl %%eax, %0;"
        :"=r"(oldValue)
        :"r"(*lockPtr)
    );
    // *lockPtr = isLocked;
    asm("xchgl %0, %1;"
        :"=r"(*lockPtr)
        :"r"(LOCK)
        );
    return oldValue;
}



void* enter(void* args){
    int* core = (int*) args;
    while (count<=20){
        printf("START\n");
        printf("%d\n", isLocked);
        while(testAndSet(&isLocked));


        // Section critique
        printf("Enter... \n\tLock = %d", isLocked);
        // Do something
        printf("\n\tDo something !\n");
        while(rand()>RAND_MAX/10000);
        count+=1;
        printf("count est = %d\n",count);
        // Lock = 0
        printf("a ce moment islocked est = %d\n",isLocked);
        unlock();
        printf("a ce moment islocked est = %d\n",isLocked);


        //printf("\n\tLock = %d\n", isLocked);
        printf("Quit...\n");
    }
}




int main ( int argc, char *argv[])
{
    int err;
    long NbrCore = strtol(argv[1], NULL, 10);//10 etant la base 10 (décimal)
    long times = 6400/NbrCore;

    pthread_t thread_T[NbrCore];


    for(int i = 0; i < NbrCore; ++i){
        err = pthread_create(&(thread_T[i]), NULL, enter, (void*)&(NbrCore));
    }

    for(int i = 0; i < NbrCore; ++i){
        err = pthread_join(thread_T[i], NULL);
    }


    return 1;
}