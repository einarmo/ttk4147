#define _GNU_SOURCE
#include "io.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define CH_1 1
#define CH_2 2
#define CH_3 3

int set_cpu(int cpu_number) {
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);
    
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}
struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}

struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}

void *fnA(void* args) {
    long ch = (long)args;
    while (1) {
        while (io_read(ch));
        io_write(ch, 0);
        usleep(5);
        io_write(ch, 1);
    }
}

void *fnB(void* args) {
    set_cpu(1);
    long ch = (long)args;
    while (1) {
        while (io_read(ch));
        io_write(ch, 0);
        usleep(5);
        io_write(ch, 1);
    }
}

void *dist(void* args) {
    set_cpu(1);
    while (1) {
        asm volatile("" ::: "memory");
    }
}

void taskA() {
    pthread_t threadHandles[3];
    for (long i = 0; i < 3; i++) {
        pthread_create(&threadHandles[i], NULL, fnA, (void*)(i+1));
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threadHandles[i], NULL);
    }
}

void taskB() {
    pthread_t threadHandles[3];
    for (long i = 0; i < 3; i++) {
        pthread_create(&threadHandles[i], NULL, fnB, (void*)(i+1));
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threadHandles[i], NULL);
    }
}

void taskC() {
    pthread_t distHandles[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&distHandles[i], NULL, dist, NULL);
    }
    pthread_t threadHandles[3];
    for (long i = 0; i < 3; i++) {
        pthread_create(&threadHandles[i], NULL, fnB, (void*)(i+1));
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threadHandles[i], NULL);
    }
}
void *fnD(void* args) {
    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);
    struct timespec period = { .tv_sec = 0, .tv_nsec = 1000 * 1000 };
    set_cpu(1);
    long ch = (long)args;
    while (1) {
        while (io_read(ch)) {
            waketime = timespec_add(waketime, period);
            clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
        }
        io_write(ch, 0);
        usleep(5);
        io_write(ch, 1);
    }
}

void taskD1() {
    pthread_t threadHandles[3];
    for (long i = 0; i < 3; i++) {
        pthread_create(&threadHandles[i], NULL, fnD, (void*)(i+1));
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threadHandles[i], NULL);
    }
}

void taskD2() {
    pthread_t distHandles[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&distHandles[i], NULL, dist, NULL);
    }
    pthread_t threadHandles[3];
    for (long i = 0; i < 3; i++) {
        pthread_create(&threadHandles[i], NULL, fnB, (void*)(i+1));
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(threadHandles[i], NULL);
    }
}

int main() {
    io_init();
    taskD2();
    
    return 0;
}
