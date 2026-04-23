#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#define N 2000000

static double a[N], b[N], c[N];

int main() {
    clock_t start_time, end_time;
    double cpu_time_used;

    for(int i = 0; i < N; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
    }

    start_time = clock();

    for(int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }

    end_time = clock();

    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    printf("Tempo de execução sequencial: %f segundos\n", cpu_time_used);
    printf("Resultado c[0]: %f\n", c[0]);

    return 0;
}