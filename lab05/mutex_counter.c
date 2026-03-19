#include <stdio.h>
#include <pthread.h>
		
#define NUM_THREADS 8
#define INCREMENTS_PER_THREAD 1000000

long long counter = 0;
pthread_mutex_t mutex; // 1. Declarar o mutex

void* thread_func(void* arg) {
    for (int i = 0; i < INCREMENTS_PER_THREAD; ++i) {
        pthread_mutex_lock(&mutex);   // 2. Travar antes de entrar na secao critica
        counter++;                    // 3. Secao Critica
        pthread_mutex_unlock(&mutex); // 4. Destravar ao sair
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL); // 5. Inicializar o mutex
    pthread_t threads[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex); // 6. Destruir o mutex
    
    long long expected = (long long)NUM_THREADS * INCREMENTS_PER_THREAD;
    printf("Valor final do contador: %lld\n", counter);
    printf("Valor esperado:          %lld\n", expected);
    if (counter == expected) {
        printf("Resultado correto!\n");
    }
    return 0;
}