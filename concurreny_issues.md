# Concurrency problems occur when multiple threads or processes attempt to access and manipulate shared resources simultaneously, leading to unpredictable results and potential system failures. Here are some common concurrency problems, along with examples and explanations:

### 1. Race Condition

A race condition occurs when the behavior of the software depends on the relative timing of events, such as the order in which threads execute. It typically happens when multiple threads access and modify shared data concurrently without proper synchronization.

#### Example:

```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;

void* increment_counter(void* arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;
    }
    return NULL;
}

int main() {
    pthread_t threads[2];
    pthread_create(&threads[0], NULL, increment_counter, NULL);
    pthread_create(&threads[1], NULL, increment_counter, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    printf("Final counter value: %d\n", counter);
    return 0;
}
```

#### Explanation:

In this example, two threads increment a shared counter variable. The expected final counter value is 200000, but due to race conditions, the actual value might be less because the threads might read and write the counter variable simultaneously, leading to lost updates.

### 2. Deadlock

A deadlock occurs when two or more threads are unable to proceed because each is waiting for the other to release a resource.

#### Example:

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock1, lock2;

void* thread1_function(void* arg) {
    pthread_mutex_lock(&lock1);
    sleep(1); // Simulate some work
    pthread_mutex_lock(&lock2);
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    return NULL;
}

void* thread2_function(void* arg) {
    pthread_mutex_lock(&lock2);
    sleep(1); // Simulate some work
    pthread_mutex_lock(&lock1);
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
    return NULL;
}

int main() {
    pthread_t threads[2];
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);

    pthread_create(&threads[0], NULL, thread1_function, NULL);
    pthread_create(&threads[1], NULL, thread2_function, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    return 0;
}
```

#### Explanation:

In this example, `thread1` locks `lock1` and then tries to lock `lock2`, while `thread2` locks `lock2` and then tries to lock `lock1`. Both threads end up waiting for the other to release the lock, resulting in a deadlock.

### 3. Livelock

Livelock occurs when threads keep responding to each other in such a way that none of them can proceed. Unlike deadlock, where threads are blocked, in livelock, threads are actively trying to resolve the issue but still make no progress.

#### Example:

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock1, lock2;
int flag1 = 0, flag2 = 0;

void* thread1_function(void* arg) {
    while (1) {
        if (!flag2) {
            pthread_mutex_lock(&lock1);
            flag1 = 1;
            if (!flag2) {
                break;
            }
            pthread_mutex_unlock(&lock1);
            flag1 = 0;
        }
    }
    // Critical section
    pthread_mutex_unlock(&lock1);
    return NULL;
}

void* thread2_function(void* arg) {
    while (1) {
        if (!flag1) {
            pthread_mutex_lock(&lock2);
            flag2 = 1;
            if (!flag1) {
                break;
            }
            pthread_mutex_unlock(&lock2);
            flag2 = 0;
        }
    }
    // Critical section
    pthread_mutex_unlock(&lock2);
    return NULL;
}

int main() {
    pthread_t threads[2];
    pthread_mutex_init(&lock1, NULL);
    pthread_mutex_init(&lock2, NULL);

    pthread_create(&threads[0], NULL, thread1_function, NULL);
    pthread_create(&threads[1], NULL, thread2_function, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    return 0;
}
```

#### Explanation:

In this example, both threads try to acquire locks based on flags that indicate the other thread's state. They keep unlocking and locking repeatedly in response to each other, resulting in livelock where they make no real progress.

### 4. Starvation

Starvation occurs when a thread is perpetually denied access to resources it needs to proceed, often because other threads monopolize the resource.

#### Example:

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock;
int counter = 0;

void* low_priority_function(void* arg) {
    while (1) {
        if (pthread_mutex_trylock(&lock) == 0) {
            counter++;
            printf("Low priority thread, Counter Value: %d\n", counter);
            pthread_mutex_unlock(&lock);
            break;
        }
    }
    return NULL;
}

void* high_priority_function(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        counter++;
        printf("High priority thread, Counter Value: %d\n", counter);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t threads[2];
    pthread_mutex_init(&lock, NULL);

    pthread_create(&threads[0], NULL, low_priority_function, NULL);
    pthread_create(&threads[1], NULL, high_priority_function, NULL);

    pthread_join(threads[0], NULL);
    pthread_cancel(threads[1]);

    pthread_mutex_destroy(&lock);
    return 0;
}
```

#### Explanation:

In this example, the high-priority thread continuously locks and unlocks the mutex, preventing the low-priority thread from ever acquiring the lock, resulting in starvation for the low-priority thread.

### 5. Priority Inversion

Priority inversion occurs when a lower-priority thread holds a lock needed by a higher-priority thread, causing the higher-priority thread to wait.

#### Example:

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock;
int shared_data = 0;

void* low_priority_function(void* arg) {
    pthread_mutex_lock(&lock);
    printf("Low priority thread acquired lock\n");
    sleep(2); // Simulate long operation
    shared_data = 1;
    pthread_mutex_unlock(&lock);
    printf("Low priority thread released lock\n");
    return NULL;
}

void* high_priority_function(void* arg) {
    printf("High priority thread waiting for lock\n");
    pthread_mutex_lock(&lock);
    printf("High priority thread acquired lock\n");
    shared_data = 2;
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t low_thread, high_thread;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&low_thread, NULL, low_priority_function, NULL);
    sleep(1); // Ensure low priority thread acquires the lock first
    pthread_create(&high_thread, NULL, high_priority_function, NULL);

    pthread_join(low_thread, NULL);
    pthread_join(high_thread, NULL);

    pthread_mutex_destroy(&lock);
    return 0;
}
```

#### Explanation:

In this example, the low-priority thread acquires the lock first and holds it for a long time. The high-priority thread has to wait for the lock, resulting in priority inversion.

### Conclusion

Concurrency problems like race conditions, deadlocks, livelocks, starvation, and priority inversion can cause significant issues in multi-threaded applications. Proper synchronization mechanisms like mutexes, semaphores, condition variables, and careful design are essential to avoid these problems and ensure correct and efficient concurrent execution.
