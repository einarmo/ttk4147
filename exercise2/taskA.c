
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

long cnt = 0;

void *fn(void* args) {
    int i;
    for (i = 0; i < 50000000; i++) {
        cnt++;
    }
    printf("local: %i, global: %ld\n", i, cnt);
    return NULL;
}

int main() {
    pthread_t threadHandle;
    pthread_t threadHandle2;
    pthread_create(&threadHandle, NULL, fn, NULL);
    pthread_create(&threadHandle2, NULL, fn, NULL);
    pthread_join(threadHandle, NULL);
    pthread_join(threadHandle2, NULL);
    return 0;
}
