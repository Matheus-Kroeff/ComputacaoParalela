#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// --- Definições de Tamanho ---
#define N_DEFAULT 1000
#define BLOCK_SIZE 32 // Tamanho do bloco para cache (teste com 16, 32 ou 64)

// --- Variáveis Globais (Matrizes) ---
double **A, **B, **C;

// --- Funções de Alocação ---
double** allocate_matrix(int N) {
    double** mat = (double**)malloc(N * sizeof(double*));
    for (int i = 0; i < N; i++) {
        mat[i] = (double*)malloc(N * sizeof(double));
    }
    return mat;
}

void initialize_matrices(int N) {
    A = allocate_matrix(N);
    B = allocate_matrix(N);
    C = allocate_matrix(N);
    
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = (double)rand() / RAND_MAX;
            B[i][j] = (double)rand() / RAND_MAX;
            C[i][j] = 0.0;
        }
    }
}

// --- VERSÃO 1: Serial IJK (Péssima localidade) ---
void mm_ijk_serial(int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j]; // Salta na memória ao ler B
            }
        }
    }
}

// --- VERSÃO 2: Serial IKJ (Boa localidade espacial) ---
void mm_ikj_serial(int N) {
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            for (int j = 0; j < N; j++) {
                C[i][j] += A[i][k] * B[k][j]; // Acessa B[k] de forma contígua
            }
        }
    }
}

// --- VERSÃO 3: Paralela IKJ (Boa localidade espacial) ---
void mm_ikj_parallel(int N) {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            for (int j = 0; j < N; j++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// --- VERSÃO 4: Paralela Blocada IKJ (Boa localidade temporal e espacial) ---
void mm_blocked_parallel(int N) {
    int i, j, k, i_block, j_block, k_block;
    
    #pragma omp parallel for private(j, k, i_block, j_block, k_block)
    for (i = 0; i < N; i += BLOCK_SIZE) {
        for (k = 0; k < N; k += BLOCK_SIZE) {
            for (j = 0; j < N; j += BLOCK_SIZE) {
                
                // Mini-multiplicacao (ikj) dentro do bloco
                for (i_block = i; i_block < i + BLOCK_SIZE && i_block < N; i_block++) {
                    for (k_block = k; k_block < k + BLOCK_SIZE && k_block < N; k_block++) {
                        for (j_block = j; j_block < j + BLOCK_SIZE && j_block < N; j_block++) {
                            C[i_block][j_block] += A[i_block][k_block] * B[k_block][j_block];
                        }
                    }
                }
            }
        }
    }
}


// --- Função Principal (Controlador) ---
int main(int argc, char** argv) {
    int N = (argc > 1) ? atoi(argv[1]) : N_DEFAULT;
    
    initialize_matrices(N);
    
    double start_time = omp_get_wtime();

    #if defined(SERIAL_IJK)
        printf("Executando Serial IJK (N=%d)\n", N);
        mm_ijk_serial(N);
    #elif defined(SERIAL_IKJ)
        printf("Executando Serial IKJ (N=%d)\n", N);
        mm_ikj_serial(N);
    #elif defined(PARALLEL_IKJ)
        printf("Executando Paralelo IKJ (N=%d, Threads=%d)\n", N, omp_get_max_threads());
        mm_ikj_parallel(N);
    #elif defined(PARALLEL_BLOCKED)
        printf("Executando Paralelo Blocado (N=%d, BS=%d, Threads=%d)\n", N, BLOCK_SIZE, omp_get_max_threads());
        mm_blocked_parallel(N);
    #else
        #error "Nenhuma versao de multiplicacao definida na compilacao."
    #endif

    double end_time = omp_get_wtime();
    printf("Tempo total: %f segundos\n", end_time - start_time);

    // Liberar memória (omitido para brevidade)
    return 0;
}