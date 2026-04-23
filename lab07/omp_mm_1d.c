#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

int main() {
    clock_t start_time, end_time;
    
    double **a = malloc(N * sizeof(double *));
    double **b = malloc(N * sizeof(double *));
    double **c = malloc(N * sizeof(double *));
    for (int i = 0; i < N; i++) {
        a[i] = malloc(N * sizeof(double));
        b[i] = malloc(N * sizeof(double));
        c[i] = malloc(N * sizeof(double));
    }

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            a[i][j] = 2.0;
            b[i][j] = 3.0;
            c[i][j] = 0.0;
        }
    }

    printf("Calculando...\n");
    start_time = clock();

    for(int i = 0; i < N; i++) {
        for(int k = 0; k < N; k++) {
            for(int j = 0; j < N; j++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    end_time = clock();
    printf("Tempo: %f segundos\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    printf("Resultado c[0][0]: %f\n", c[0][0]);

    for (int i = 0; i < N; i++) {
        free(a[i]); free(b[i]); free(c[i]);
    }
    free(a); free(b); free(c);

    return 0;
}