#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "mybarrier.h"
#include <stdint.h>

#define NTHREADS 5

static mybarrier_t *barrier;

void *thread_fn(void *arg)
{
    intptr_t id = (intptr_t)arg;
    int ms = rand() % 2000;
    usleep(ms * 1000);
    printf("Thread %ld: before barrier 1\n", (long)id);
    fflush(stdout);

    if (mybarrier_wait(barrier) != 0) {
        fprintf(stderr, "Thread %ld: mybarrier_wait failed\n", (long)id);
        return NULL;
    }

    printf("Thread %ld: after barrier\n", (long)id);
    fflush(stdout);

    return NULL;
}

int main(void)
{
    pthread_t threads[NTHREADS];
    int i;

    srand((unsigned)time(NULL));

    barrier = mybarrier_init(NTHREADS);
    if (!barrier) {
        fprintf(stderr, "Failed to create barrier\n");
        return 1;
    }

    for (i = 0; i < NTHREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_fn, (void *)(intptr_t)i) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    if (mybarrier_destroy(barrier) != 0) {
        fprintf(stderr, "Failed to destroy barrier\n");
        return 1;
    }

    printf("All threads completed.\n");
    return 0;
}
