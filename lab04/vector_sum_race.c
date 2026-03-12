#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define VECTOR_SIZE 10000000
#define N_THREADS 4

long long global_sum = 0;
int *vector;

typedef struct {
    int start;
    int end;
} ThreadData;

void* sum_vector(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = data->start; i < data->end; i++) {
        // O PROBLEMA ESTÁ AQUI:
        global_sum += vector[i];
    }
    return NULL;
}

int main() {
    vector = malloc(VECTOR_SIZE * sizeof(int));
    long long expected_sum = 0;

    // Inicializa o vetor e calcula a soma esperada
    for (int i = 0; i < VECTOR_SIZE; i++) {
        vector[i] = 1; // Usando 1 para facilitar a conferência
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

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Soma Calculada: %lld\n", global_sum);
    printf("Soma Esperada:  %lld\n", expected_sum);
    printf("Diferença:      %lld\n", expected_sum - global_sum);

    free(vector);
    return 0;
}