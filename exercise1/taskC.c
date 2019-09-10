#include "common.h"
#include <string.h>
#include <stdio.h>

void get_overhead() {
    struct tms t;
    for(long int i = 0; i < 10*1000*1000; i++){
        times(&t);
    }
}

void get_resolution() {
    int ns_max = 250;
    int histogram[ns_max];

    memset(histogram, 0, sizeof(int)*ns_max);
    struct timespec ts1;
    struct timespec ts2;
    for(int i = 0; i < 10*1000*1000; i++){
        clock_gettime(CLOCK_MONOTONIC, &ts1);
        sched_yield();
        clock_gettime(CLOCK_MONOTONIC, &ts2);
        long t1 = ts1.tv_nsec;
        long t2 = ts2.tv_nsec;
        int ns = (t2 - t1);
    
        if(ns >= 0 && ns < ns_max){
            histogram[ns]++;
        }
    }

    for(int i = 0; i < ns_max; i++){
        printf("%d\n", histogram[i]);
    }

}


int main() {
    get_resolution();
    return 0;
}
