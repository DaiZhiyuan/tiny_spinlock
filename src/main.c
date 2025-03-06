#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "spinlock.h"

#define NUM_THREADS 256
#define INCREMENTS 10000

volatile int shared_counter = 0;

arm64_spinlock_t spinlock = {0, 0};

void *thread_func(void *arg)
{
    int i;

    for(i = 0; i < INCREMENTS; i++) {
        arm64_spin_lock(&spinlock);
        shared_counter++;
        arm64_spin_unlock(&spinlock);
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    int i;

    for(i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_func, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for(i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Expected Value: %d\n", NUM_THREADS * INCREMENTS);
    printf("Real Value: %d\n", shared_counter);

    return 0;
}

