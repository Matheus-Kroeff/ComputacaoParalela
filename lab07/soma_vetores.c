#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 2000000

double a[N], b[N], c[N];

int main() {

    double start_time, end_time;

	
	#pragma omp parallel for
    for(int i = 0; i < N; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
    }

    start_time = omp_get_wtime();

    #pragma omp parallel for
    for(int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }

    end_time = omp_get_wtime();

    printf("Tempo de execução paralela: %f segundos\n", end_time - start_time);

	
	return 0;
}