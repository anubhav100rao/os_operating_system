# Some well-known concurrency problems using synchronization primitives like mutexes and semaphores in C. The examples include the producer-consumer problem, the readers-writers problem, and the dining philosophers problem.

### 1. Producer-Consumer Problem

The producer-consumer problem involves two types of threads, producers and consumers, that share a fixed-size buffer. Producers add items to the buffer, and consumers remove items from the buffer. The solution ensures that producers don't add items to a full buffer and consumers don't remove items from an empty buffer.

#### Solution using Semaphores:

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int in = 0, out = 0;
sem_t empty, full, mutex;

void* producer(void* arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = i;
        in = (in + 1) % BUFFER_SIZE;
        printf("Produced: %d\n", i);

        sem_post(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < 20; i++) {
        sem_wait(&full);
        sem_wait(&mutex);

        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        printf("Consumed: %d\n", item);

        sem_post(&mutex);
        sem_post(&empty);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    return 0;
}
```

### 2. Readers-Writers Problem

The readers-writers problem involves multiple threads trying to read from and write to a shared resource. The solution must ensure that no reader is kept waiting unless a writer has already acquired the resource, and no writer is kept waiting longer than necessary.

#### Solution using Mutex and Condition Variables (First Readers-Writers Problem):

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex, write_mutex;
int read_count = 0;

void* reader(void* arg) {
    pthread_mutex_lock(&mutex);
    read_count++;
    if (read_count == 1) {
        pthread_mutex_lock(&write_mutex);
    }
    pthread_mutex_unlock(&mutex);

    // Reading
    printf("Reader %d is reading\n", *(int*)arg);

    pthread_mutex_lock(&mutex);
    read_count--;
    if (read_count == 0) {
        pthread_mutex_unlock(&write_mutex);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* writer(void* arg) {
    pthread_mutex_lock(&write_mutex);

    // Writing
    printf("Writer %d is writing\n", *(int*)arg);

    pthread_mutex_unlock(&write_mutex);
    return NULL;
}

int main() {
    pthread_t readers[5], writers[2];
    int ids[5] = {0, 1, 2, 3, 4};

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&write_mutex, NULL);

    for (int i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, reader, &ids[i]);
    }
    for (int i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer, &ids[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&write_mutex);
    return 0;
}
```

### 3. Dining Philosophers Problem

The dining philosophers problem involves philosophers sitting at a table, each needing two forks to eat. The solution must ensure that no philosopher starves and deadlock is avoided.

#### Solution using Mutexes:

```c
#include <stdio.h>
#include <pthread.h>

#define N 5

pthread_mutex_t forks[N];

void* philosopher(void* arg) {
    int id = *(int*)arg;
    int left = id;
    int right = (id + 1) % N;

    while (1) {
        // Think
        printf("Philosopher %d is thinking\n", id);

        // Pick up forks
        pthread_mutex_lock(&forks[left]);
        pthread_mutex_lock(&forks[right]);

        // Eat
        printf("Philosopher %d is eating\n", id);

        // Put down forks
        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);
    }
    return NULL;
}

int main() {
    pthread_t philosophers[N];
    int ids[N];

    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    return 0;
}
```

### Explanation of Solutions

1. **Producer-Consumer Problem**:

    - **Semaphores**: `empty` tracks the number of empty slots, `full` tracks the number of filled slots, and `mutex` ensures mutual exclusion when accessing the buffer.
    - **Producer**: Waits for an empty slot (`sem_wait(&empty)`) and then acquires the mutex (`sem_wait(&mutex)`) to add an item to the buffer. It then signals that a slot is filled (`sem_post(&full)`).
    - **Consumer**: Waits for a filled slot (`sem_wait(&full)`) and then acquires the mutex (`sem_wait(&mutex)`) to remove an item from the buffer. It then signals that a slot is empty (`sem_post(&empty)`).

2. **Readers-Writers Problem**:

    - **Mutex and Condition Variables**: Use `mutex` to protect the read count and `write_mutex` to ensure exclusive access for writers.
    - **Reader**: Increments the read count and acquires the `write_mutex` if it is the first reader. Decrements the read count and releases the `write_mutex` if it is the last reader.
    - **Writer**: Acquires the `write_mutex` to get exclusive access and releases it after writing.

3. **Dining Philosophers Problem**:
    - **Mutexes**: Each philosopher tries to pick up the left fork and then the right fork. After eating, they release the forks.
    - This solution avoids deadlock by ensuring that philosophers pick up forks in a specific order.

These solutions address common concurrency problems using synchronization primitives to ensure correct and efficient access to shared resources in multi-threaded applications.
