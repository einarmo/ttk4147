#define NUM_PHILOSOPHERS 5
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t forks[NUM_PHILOSOPHERS];

void eat() {
    int time = 100000000; //rand() % 10000000; 
    for (long i = 0; i < time; i++) {
    	asm volatile("" ::: "memory");
    }
}

pthread_mutex_t* getFork(long num) {
    return &forks[num < 0 ? NUM_PHILOSOPHERS - 1 : num % NUM_PHILOSOPHERS];
}

void* philosopher(void* args) {
    // Get left fork
    long num = (int)args;
    while (1) {
	eat();
        pthread_mutex_lock(getFork(num - 1));
        eat();
	pthread_mutex_lock(getFork(num + 1));
        eat();
	printf("Philospher %ld eating\n", num);
        pthread_mutex_unlock(getFork(num - 1));
        pthread_mutex_unlock(getFork(num + 1));
    }
}

int main() {
    time_t t;
    srand((unsigned) time (&t));
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }
    pthread_t threadHandles[NUM_PHILOSOPHERS];
    for (long i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&threadHandles[i], NULL, philosopher, (void*)i);
    }
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threadHandles[i], NULL);
    }
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    return 0;
}
