#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void allocate_big() {
    long xy_size = 1000*1000*1000;
    long x_dim = 10000;
    long y_dim = xy_size/x_dim;
    long** matrix = malloc(y_dim*sizeof(long*));
    
    for (long y = 0; y < y_dim; y++) {
        matrix[y] = malloc(x_dim*sizeof(long));
        memset(matrix[y], 0, x_dim*sizeof(long));
    }

    printf("Allocation complete (press any key to continue...)\n");
    getchar();
}

int main() {
    allocate_big();
    return 0;
}
