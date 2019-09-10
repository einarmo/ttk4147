#include <stdio.h>
#include <string.h>
#include "common.h"

void busy_wait_times() {
    struct tms buff;
    long int start = times(&buff);
    clock_t tps = sysconf(_SC_CLK_TCK);
    
    while (times(&buff) - start <= tps);
}

void busy_wait_gettime(){
    struct timespec t;
    t.tv_sec = 1;
    t.tv_nsec = 0;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec then = timespec_add(now, t);
    
    while(timespec_cmp(now, then) < 0){
        for(int i = 0; i < 10000; i++){}
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
}

void wait_sleep() {
    sleep(1);
}

int main() {
    wait_sleep();
    return 0;
}
