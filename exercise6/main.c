#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>
#include "io.h"
#include <pthread.h>
#include <unistd.h>

#define DURATION 60000000000

void timeout(long endTime) {
    if (rt_timer_read() > endTime) {
        rt_printf("Time expired\n");
        rt_task_delete(NULL);
    }
    if (rt_task_yield()) {
        rt_printf("Task failed to yield\n");
        rt_task_delete(NULL);
    }
}

int set_cpu(int cpu_number) {
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);
    
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void fnA(void* args) {
    unsigned long endTime = rt_timer_read() + DURATION;
    long ch = (long)args;
    rt_printf("Start task on channel %i\n", ch);
    while (1) {
        while (io_read(ch)) {
            timeout(endTime);
        }
        io_write(ch, 0);
        rt_timer_spin(5000);
        io_write(ch, 1);
    }
}

void fnB(void* args) {
    long ch = (long)args;
    while (1) {
        rt_task_wait_period(NULL);
        if (!io_read(ch)) {
            io_write(ch, 0);
            rt_timer_spin(5000);
            io_write(ch, 1);
        }
    }
}

void *dist(void* args) {
    set_cpu(1);
    while (1) {
        asm volatile("" ::: "memory");
    }
}        


void taskA() {
    RT_TASK tasks[3];
    for (long i = 0; i < 3; i++) {
        rt_task_create(&tasks[i], NULL, 0, 99, T_CPU(1) | T_JOINABLE);
    }
    for (long i = 0; i < 3; i++) {
        rt_task_start(&tasks[i], &fnA, (void*)(i+1));
    }
    printf("Started tasks");
    for (long i = 0; i < 3; i++) {
        rt_task_join(&tasks[i]);
    }
    printf("Tasks ended");
}

void taskA2() {
    RT_TASK tasks[3];
    for (long i = 0; i < 3; i++) {
        rt_task_create(&tasks[i], NULL, 0, 99, T_CPU(1) | T_JOINABLE);
    }
    
    for (long i = 0; i < 3; i++) {
        rt_task_start(&tasks[i], &fnA, (void*)(i+1));
    }

    pthread_t distHandles[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&distHandles[i], NULL, dist, NULL);
    }

    for (long i = 0; i < 3; i++) {
        rt_task_join(&tasks[i]);
    }
}

void taskB2() {
    RT_TASK tasks[3];
    for (long i = 0; i < 3; i++) {
        rt_task_create(&tasks[i], NULL, 0, 99, T_CPU(1) | T_JOINABLE);
        rt_task_set_periodic(&tasks[i], TM_NOW, 1000000); 
    }
    
    for (long i = 0; i < 3; i++) {
        rt_task_start(&tasks[i], &fnB, (void*)(i+1));
    }

    pthread_t distHandles[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&distHandles[i], NULL, dist, NULL);
    }

    for (long i = 0; i < 3; i++) {
        rt_task_join(&tasks[i]);
    }
}

void taskB() {
    RT_TASK tasks[3];
    for (long i = 0; i < 3; i++) {
        rt_task_create(&tasks[i], NULL, 0, 99, T_CPU(1) | T_JOINABLE);
        rt_task_set_periodic(&tasks[i], TM_NOW, 1000000); 
    }
    
    for (long i = 0; i < 3; i++) {
        rt_task_start(&tasks[i], &fnB, (void*)(i+1));
    }

    for (long i = 0; i < 3; i++) {
        rt_task_join(&tasks[i]);
    }
}

int main() {
    mlockall(MCL_CURRENT|MCL_FUTURE);
    io_init();
    rt_print_auto_init(1);
    taskB();
    return 0;
}
