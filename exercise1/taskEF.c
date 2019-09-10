#include "array.h"
#include <stdio.h>
#include <string.h>

void print_array(Array *a) {
    for(int i = a->front; i < a->back; i++) {
        printf("%lu, ", a->data[i]);
    }
}

int main() {
    Array a = array_new(100);
    printf("%p - %p\n", a.data, a.data + sizeof(long*)*a.capacity);
    for (int i = 0; i < 100000; i++) {
        array_insertBack(&a, i); 
    }
    // print_array(&a);
    array_destroy(a);
    return 0;
}
