#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define VECTOR_SIZE 10000000
#define N_THREADS 4

int *vector;

typedef struct {
    int start;
    int end;
} ThreadData;

void* sum_vector(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    long long *local_sum = malloc(sizeof(long long));
    *local_sum = 0;

    for (int i = data->start; i < data->end; i++) {
        *local_sum += vector[i];
    }
    pthread_exit((void*)local_sum);
}

int main() {
    vector = malloc(VECTOR_SIZE * sizeof(int));
    long long expected_sum = 0;
    long long final_global_sum = 0;

    for (int i = 0; i < VECTOR_SIZE; i++) {
        vector[i] = 1; 
        expected_sum += vector[i];
    }

    pthread_t threads[N_THREADS];
    ThreadData tdata[N_THREADS];
    int slice = VECTOR_SIZE / N_THREADS;

    for (int i = 0; i < N_THREADS; i++) {
        tdata[i].start = i * slice;
        tdata[i].end = (i == N_THREADS - 1) ? VECTOR_SIZE : (i + 1) * slice;
        pthread_create(&threads[i], NULL, sum_vector, &tdata[i]);
    }

    void *return_value;
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], &return_value);
        
        long long *res = (long long*)return_value;
        final_global_sum += *res; 
        free(res); 
    } 

    printf("Soma Calculada: %lld\n", final_global_sum);
    printf("Soma Esperada:  %lld\n", expected_sum);
    printf("Diferença:      %lld\n", expected_sum - final_global_sum);

    free(vector);
    return 0;
}