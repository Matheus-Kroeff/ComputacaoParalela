#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdbool.h>

// --- Funções Auxiliares ---
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int* create_array(int N) {
    int* A = (int*)malloc(N * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        A[i] = rand() % (N * 10);
    }
    return A;
}

bool is_sorted(int A[], int N) {
    for (int i = 0; i < N - 1; i++) {
        if (A[i] > A[i+1]) return false;
    }
    return true;
}

// --- VERSÃO 1: Odd-Even Sort (Serial) ---
void odd_even_serial(int A[], int N) {
    for (int fase = 0; fase < N; fase++) {
        if (fase % 2 == 0) { // Fase Par
            for (int i = 0; i < N - 1; i += 2) {
                if (A[i] > A[i+1]) swap(&A[i], &A[i+1]);
            }
        } else { // Fase Impar
            for (int i = 1; i < N - 1; i += 2) {
                if (A[i] > A[i+1]) swap(&A[i], &A[i+1]);
            }
        }
    }
}

// --- VERSÃO 2: Odd-Even Sort (Paralelo - Aula 06) ---
void odd_even_parallel(int A[], int N) {
    int fase, i;
    #pragma omp parallel private(fase, i) shared(A, N)
    {
        for (fase = 0; fase < N; fase++) {
            if (fase % 2 == 0) { // Fase Par
                #pragma omp for
                for (i = 0; i < N - 1; i += 2) {
                    if (A[i] > A[i+1]) swap(&A[i], &A[i+1]);
                }
            } else { // Fase Impar
                #pragma omp for
                for (i = 1; i < N - 1; i += 2) {
                    if (A[i] > A[i+1]) swap(&A[i], &A[i+1]);
                }
            }
            // Barreira implicita do 'omp for' sincroniza as fases
        }
    }
}


// --- VERSÃO 3: Quicksort (Serial) ---
int partition_serial(int A[], int low, int high) {
    int pivot = A[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (A[j] < pivot) {
            i++;
            swap(&A[i], &A[j]);
        }
    }
    swap(&A[i + 1], &A[high]);
    return (i + 1);
}

void quicksort_serial_recursive(int A[], int low, int high) {
    if (low < high) {
        int pi = partition_serial(A, low, high);
        quicksort_serial_recursive(A, low, pi - 1);
        quicksort_serial_recursive(A, pi + 1, high);
    }
}

void quicksort_serial(int A[], int N) {
    quicksort_serial_recursive(A, 0, N - 1);
}


// --- VERSÃO 4: Quicksort (Paralelo com Tasks - Aula 05) ---
#define CUTOFF 1000 // Limite para parar de criar tarefas

int partition_parallel(int A[], int low, int high) {
    // Particionamento eh identico ao serial
    int pivot = A[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (A[j] < pivot) {
            i++;
            swap(&A[i], &A[j]);
        }
    }
    swap(&A[i + 1], &A[high]);
    return (i + 1);
}

void quicksort_parallel_recursive(int A[], int low, int high) {
    if (low < high) {
        int pi = partition_parallel(A, low, high);
        
        // Se o problema for pequeno, executa serial (cutoff)
        if (high - low < CUTOFF) {
            quicksort_parallel_recursive(A, low, pi - 1);
            quicksort_parallel_recursive(A, pi + 1, high);
        } else {
            // Cria uma tarefa para a primeira metade
            #pragma omp task
            {
                quicksort_parallel_recursive(A, low, pi - 1);
            }
            
            // A thread atual cuida da segunda metade
            quicksort_parallel_recursive(A, pi + 1, high);
            
            // (taskwait nao eh necessario aqui, 
            // pois a thread pai ja faz metade do trabalho)
        }
    }
}

void quicksort_parallel(int A[], int N) {
    // Cria a equipe de threads UMA VEZ
    #pragma omp parallel
    {
        // Apenas UMA thread (a master) inicia a chamada raiz
        #pragma omp single
        {
            quicksort_parallel_recursive(A, 0, N - 1);
        }
    } // Barreira implicita: o programa espera todas as tarefas terminarem
}


// --- Função Principal (Controlador) ---
int main(int argc, char** argv) {
    int N = (argc > 1) ? atoi(argv[1]) : 50000;
    
    int* A = create_array(N);
    int* A_serial_ref = (int*)malloc(N * sizeof(int)); // Para o quicksort serial
    
    // Flag para decidir qual versao serial rodar
    bool is_serial = (omp_get_max_threads() == 1);

    double start_time = omp_get_wtime();

    #if defined(ODD_EVEN)
        if (is_serial) {
            printf("Executando Odd-Even Serial (N=%d)\n", N);
            odd_even_serial(A, N);
        } else {
            printf("Executando Odd-Even Paralelo (N=%d, Threads=%d)\n", N, omp_get_max_threads());
            odd_even_parallel(A, N);
        }
    #elif defined(QUICKSORT)
        if (is_serial) {
             printf("Executando Quicksort Serial (N=%d)\n", N);
             quicksort_serial(A, N);
        } else {
            printf("Executando Quicksort Paralelo (N=%d, Threads=%d)\n", N, omp_get_max_threads());
            quicksort_parallel(A, N);
        }
    #else
        #error "Nenhuma versao de ordenacao definida na compilacao."
    #endif

    double end_time = omp_get_wtime();
    printf("Tempo total: %f segundos\n", end_time - start_time);
    
    printf("Verificando... %s\n", is_sorted(A, N) ? "ORDENADO CORRETAMENTE" : "ERRO NA ORDENACAO");

    free(A);
    free(A_serial_ref);
    return 0;
}