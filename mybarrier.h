#ifndef _MYBARRIER_H
#define _MYBARRIER_H

#include <pthread.h>
#include <errno.h>

typedef struct _mybarrier_t{
    int count;                  /* number of threads required to pass the barrier */
    int waiting;                /* number of threads currently waiting */
    int active;                 /* number of threads currently inside wait() */
    int released;               /* 1 when barrier has been released (single-use) */
    int aborted;                /* 1 when barrier aborted due to destroy */
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t destroy_cond; /* signaled when active drops to 0 so destroy can proceed */
}mybarrier_t;


mybarrier_t * mybarrier_init(unsigned int count);
int mybarrier_destroy(mybarrier_t * barrier);
int mybarrier_wait(mybarrier_t * barrier);

#endif
