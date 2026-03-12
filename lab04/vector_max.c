#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N_THREADS 4
#define VECTOR_SIZE 100000

int *vector;

typedef struct {
    int start;
    int end;
} ThreadData;

void *max_vector(void *arg) {
    ThreadData* data = (ThreadData*)arg;

    int *local_max = malloc(sizeof(int));
    *local_max = vector[data -> start];
    for (int i = data->start + 1; i < data->end; i++) {
        if (vector[i] > *local_max) {
            *local_max = vector[i];
        }
    }
    pthread_exit((void*)local_max);
}

int main() {

    srand(time(NULL));

    vector = malloc(VECTOR_SIZE * sizeof(int));

    for(int i = 0; i < VECTOR_SIZE; i++) {
        vector[i] = rand() % 1000000;
    }

    pthread_t threads[N_THREADS];
    ThreadData tdata[N_THREADS];
    int slice = VECTOR_SIZE / N_THREADS;

    for (int i = 0; i < N_THREADS; i++) {
        tdata[i].start = i * slice;
        tdata[i].end = (i == N_THREADS - 1) ? VECTOR_SIZE : (i + 1) * slice;
        pthread_create(&threads[i], NULL, max_vector, &tdata[i]);
    }

    int global_max = -1;
    void *return_value;

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], &return_value);
        
        int *local_max = (int*)return_value;
        printf("Thread %d encontrou max local: %d\n", i, *local_max);

        if (*local_max > global_max) {
            global_max = *local_max;
        }

        free(local_max); 
    }

    printf("\n>>> Máximo Global Final: %d <<<\n", global_max);

    free(vector);
    return 0;


}