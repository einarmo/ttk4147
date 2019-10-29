#include <native/task.h>
#include <native/timer.h>
#include <sys/mman.h>
#include <rtdk.h>
#include <native/sem.h>
#include <native/mutex.h>

RT_SEM semA;
RT_SEM semResource;
RT_MUTEX mutexResource;

RT_MUTEX mutexA;
RT_MUTEX mutexB;

typedef struct taskData {
    int pri;
    RT_TASK *task;
    long threadNum;
} taskData;

void busy_wait_us(unsigned long delay) {
    for (; delay > 0; delay--) {
        rt_timer_spin(1000);
    }
}

void lock_mutex(int num, taskData *data) {
    RT_MUTEX *mutex = num == 1 ? &mutexA : &mutexB;
    rt_mutex_acquire(mutex, TM_INFINITE);
    rt_task_set_priority(data->task, 11);
}

void release_mutex(int num, taskData *data) {
    RT_MUTEX *mutex = num == 1 ? &mutexA : &mutexB;
    rt_mutex_release(mutex);
    rt_task_set_priority(data->task, data->pri);
}


void fnE(void* args) {
    taskData data = *((taskData*)args);
    rt_sem_p(&semA, TM_INFINITE);
    
    if (data.threadNum == 0) {
        lock_mutex(1, &data);
        busy_wait_us(30000);
        lock_mutex(2, &data);
        busy_wait_us(30000);
        release_mutex(2, &data);
        release_mutex(1, &data);
        busy_wait_us(10000);
    } else if (data.threadNum == 1) {
        rt_task_sleep(10000000);
        lock_mutex(2, &data);
        busy_wait_us(10000);
        lock_mutex(1, &data);
        busy_wait_us(20000);
        release_mutex(1, &data);
        release_mutex(2, &data);
        busy_wait_us(10000);
    }
    rt_printf("Task %i done\n", data.threadNum);
}


void fnD(void* args) {
    long num = (long)args;
    rt_sem_p(&semA, TM_INFINITE);
    
    if (num == 0) {
        rt_mutex_acquire(&mutexA, TM_INFINITE);
        busy_wait_us(30000);
        rt_mutex_acquire(&mutexB, TM_INFINITE);
        busy_wait_us(30000);
        rt_mutex_release(&mutexB);
        rt_mutex_release(&mutexA);
        busy_wait_us(10000);
    } else if (num == 1) {
        rt_task_sleep(10000000);
        rt_mutex_acquire(&mutexB, TM_INFINITE);
        busy_wait_us(10000);
        rt_mutex_acquire(&mutexA, TM_INFINITE);
        busy_wait_us(20000);
        rt_mutex_release(&mutexA);
        rt_mutex_release(&mutexB);
        busy_wait_us(10000);
    }
    rt_printf("Task %i done\n", num);
}

void fnC(void* args) {
    long num = (long)args;
    rt_sem_p(&semA, TM_INFINITE); 
    long start = rt_timer_read();

    if (num == 0) {
        rt_mutex_acquire(&mutexResource, TM_INFINITE);
        busy_wait_us(30000);
        long time = rt_timer_read() - start;
        rt_printf("Task low releasing after %i ns\n", time);
        rt_mutex_release(&mutexResource);
    } else if (num == 1) {
        rt_task_sleep(10000000); 
        busy_wait_us(50000);
        long time = rt_timer_read() - start;
        rt_printf("Task medium done after %i ns\n", time);
    } else if (num == 2) {
        rt_task_sleep(20000000);
        rt_mutex_acquire(&mutexResource, TM_INFINITE);
        busy_wait_us(20000);
        long time = rt_timer_read() - start;
        rt_printf("Task high releasing after %i ns\n", time);
        rt_mutex_release(&mutexResource);
    }
}

void fnB(void* args) {
    long num = (long)args;
    rt_sem_p(&semA, TM_INFINITE); 
    long start = rt_timer_read();

    if (num == 0) {
        rt_sem_p(&semResource, TM_INFINITE);
        busy_wait_us(30000);
        long time = rt_timer_read() - start;
        rt_printf("Task low releasing after %i ns\n", time);
        rt_sem_v(&semResource);
    } else if (num == 1) {
        rt_task_sleep(10000000); 
        busy_wait_us(50000);
        long time = rt_timer_read() - start;
        rt_printf("Task medium done after %i ns\n", time);
    } else if (num == 2) {
        rt_task_sleep(20000000);
        rt_sem_p(&semResource, TM_INFINITE);
        busy_wait_us(20000);
        long time = rt_timer_read() - start;
        rt_printf("Task high releasing after %i ns\n", time);
        rt_sem_v(&semResource);
    }
}

void fnA(void* args) {
    long num = (long)args;
    rt_sem_p(&semA, TM_INFINITE);
    rt_printf("Hello from thread %i\n", num);
}

void taskE() {
    rt_sem_create(&semA, "semA", 0, S_PRIO);
    rt_mutex_create(&mutexA, "mutexA");
    rt_mutex_create(&mutexB, "mutexB");
    RT_TASK tasks[2];
    taskData data[2];
    for (int i = 0; i < 2; i++) {
        rt_task_create(&tasks[i], NULL, 0, 10 + i, T_CPU(1) | T_JOINABLE);
    }
    for (long i = 0; i < 2; i++) {
        data[i].task = &tasks[i];
        data[i].pri = 10 + i;
        data[i].threadNum = i;
        rt_task_start(&tasks[i], &fnE, (void*)&data[i]);
    }
    rt_task_shadow(NULL, "main", 20, T_CPU(1));
    rt_task_sleep(100000000);
    rt_sem_broadcast(&semA);
    rt_task_sleep(100000000);
    for (int i = 0; i < 2; i++) {
        rt_task_join(&tasks[i]);
    }
    rt_sem_delete(&semA);
    rt_mutex_delete(&mutexA);
    rt_mutex_delete(&mutexB);
}

void taskD() {
    rt_sem_create(&semA, "semA", 0, S_PRIO);
    rt_mutex_create(&mutexA, "mutexA");
    rt_mutex_create(&mutexB, "mutexB");
    RT_TASK tasks[2];
    for (int i = 0; i < 2; i++) {
        rt_task_create(&tasks[i], NULL, 0, 10 + i, T_CPU(1) | T_JOINABLE);
    }
    for (long i = 0; i < 2; i++) {
        rt_task_start(&tasks[i], &fnD, (void*)i);
    }
    rt_task_shadow(NULL, "main", 20, T_CPU(1));
    rt_task_sleep(100000000);
    rt_sem_broadcast(&semA);
    rt_task_sleep(100000000);
    for (int i = 0; i < 2; i++) {
        rt_task_join(&tasks[i]);
    }
    rt_sem_delete(&semA);
    rt_mutex_delete(&mutexA);
    rt_mutex_delete(&mutexB);
}
void taskC() {
    rt_sem_create(&semA, "semA", 0, S_PRIO);
    rt_mutex_create(&mutexResource, "mutexResource");
    RT_TASK tasks[3];
    for (int i = 0; i < 3; i++) {
        rt_task_create(&tasks[i], NULL, 0, 10 + i, T_CPU(1) | T_JOINABLE);
    }
    for (long i = 0; i < 3; i++) {
        rt_task_start(&tasks[i], &fnC, (void*)i);
    }
    rt_task_shadow(NULL, "main", 20, T_CPU(1));
    rt_task_sleep(100000000);
    rt_sem_broadcast(&semA);
    rt_task_sleep(100000000);
    for (int i = 0; i < 3; i++) {
        rt_task_join(&tasks[i]);
    }
    rt_sem_delete(&semA);
    rt_mutex_delete(&mutexResource);
}

void taskB() {
    rt_sem_create(&semA, "semA", 0, S_PRIO);
    rt_sem_create(&semResource, "semResource", 1, S_PRIO);
    RT_TASK tasks[3];
    for (int i = 0; i < 3; i++) {
        rt_task_create(&tasks[i], NULL, 0, 10 + i, T_CPU(1) | T_JOINABLE);
    }
    for (long i = 0; i < 3; i++) {
        rt_task_start(&tasks[i], &fnB, (void*)i);
    }
    rt_task_shadow(NULL, "main", 20, T_CPU(1));
    rt_task_sleep(100000000);
    rt_sem_broadcast(&semA);
    rt_task_sleep(100000000);
    for (int i = 0; i < 3; i++) {
        rt_task_join(&tasks[i]);
    }
    rt_sem_delete(&semA);
    rt_sem_delete(&semResource);
}

void taskA() {
    rt_sem_create(&semA, "semA", 0, S_PRIO);
    RT_TASK task1;
    RT_TASK task2;
    rt_task_create(&task1, "task1", 0, 10, T_CPU(1) | T_JOINABLE);
    rt_task_create(&task2, "task2", 0, 11, T_CPU(1) | T_JOINABLE);
    rt_task_start(&task1, &fnA, (void*)1);
    rt_task_start(&task2, &fnA, (void*)2);
    rt_task_shadow(NULL, "main", 12, T_CPU(1));
    
    rt_task_sleep(100000000);
    rt_sem_broadcast(&semA);
    rt_task_sleep(100000000);
    rt_task_join(&task1);
    rt_task_join(&task2);
    rt_sem_delete(&semA);
}



int main() {
    mlockall(MCL_CURRENT|MCL_FUTURE);
    rt_print_auto_init(1);
    taskE();
    return 0;
}
