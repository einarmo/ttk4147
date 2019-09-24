
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

long cnt = 0;

sem_t mutex;

void *fn(void* args) {
    int i;
    for (i = 0; i < 50000000; i++) {
        sem_wait(&mutex);
        cnt++;
        sem_post(&mutex);
    }
    printf("local: %i, global: %ld\n", i, cnt);
    return NULL;
}

int main() {
    sem_init(&mutex, 0, 1);
    pthread_t threadHandle;
    pthread_t threadHandle2;
    pthread_create(&threadHandle, NULL, fn, NULL);
    pthread_create(&threadHandle2, NULL, fn, NULL);
    pthread_join(threadHandle, NULL);
    pthread_join(threadHandle2, NULL);
    sem_destroy(&mutex);
    return 0;
}
