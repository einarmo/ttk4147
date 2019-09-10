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
    int ns_max = 50;
    int histogram[ns_max];

    memset(histogram, 0, sizeof(int)*ns_max);
    clock_t tps = sysconf(_SC_CLK_TCK);
    for(int i = 0; i < 10*1000*1000; i++){
        struct tms buff;
        time_t t1 = times(&buff);
        time_t t2 = times(&buff);
        int ns = (t2 - t1) * (1000000000 / tps);
    
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
