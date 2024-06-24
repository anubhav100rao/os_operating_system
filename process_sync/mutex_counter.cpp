#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t lock;
int counter = 0;

void* thread_function(void* arg) {
    pthread_mutex_lock(&lock);
    counter++;
    printf("Thread %d, Counter Value: %d\n", *(int*)arg, counter);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t threads[10];
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < 10; i++) {
        int* arg = (int*)malloc(sizeof(int));
        *arg = i;
        pthread_create(&threads[i], NULL, thread_function, arg);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
