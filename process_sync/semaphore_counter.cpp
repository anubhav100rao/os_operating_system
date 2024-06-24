#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

sem_t semaphore;
int counter = 0;

void* thread_function(void* arg) {
    sem_wait(&semaphore);
    counter++;
    printf("Thread %d, Counter Value: %d\n", *(int*)arg, counter);
    sem_post(&semaphore);
    return NULL;
}

int main() {
    pthread_t threads[10];
    sem_init(&semaphore, 0, 1);

    for (int i = 0; i < 10; i++) {
        int* arg = (int*)malloc(sizeof(int));
        *arg = i;
        pthread_create(&threads[i], NULL, thread_function, arg);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&semaphore);
    return 0;
}
