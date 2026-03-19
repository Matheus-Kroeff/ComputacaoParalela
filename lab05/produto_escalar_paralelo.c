#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>

#define NUM_THREADS 8
#define VECTOR_SIZE 100000

typedef struct {
    int start;
    int end;
} ThreadData;

long long global_sum = 0;
int *vector1;
int *vector2;

pthread_mutex_t mutex; 

void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long long local_sum = 0; 

    for (int i = data->start; i < data->end; i++) {
        local_sum += (long long)vector1[i] * vector2[i];
    }

    pthread_mutex_lock(&mutex);   // Travar antes de entrar na secao critica
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex); // Destravar ao sair
    
    return NULL; 
}

int main() {
    pthread_mutex_init(&mutex, NULL); // 5. Inicializar o mutex

    vector1 = malloc(VECTOR_SIZE * sizeof(int));
    vector2 = malloc(VECTOR_SIZE * sizeof(int));

    for (int i = 0; i < VECTOR_SIZE; i++) {
        vector1[i] = 2; 
        vector2[i] = 2;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData t_data[NUM_THREADS];
    int slice_size = VECTOR_SIZE / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        t_data[i].start = i * slice_size;
        t_data[i].end = (i == NUM_THREADS - 1) ? VECTOR_SIZE : (i + 1) * slice_size;
        pthread_create(&threads[i], NULL, thread_func, &t_data[i]);
    }


    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        
    } 

    pthread_mutex_destroy(&mutex); // 6. Destruir o mutex

    printf("Resultado Global: %lld\n", global_sum);

    free(vector1);
    free(vector2);

}


