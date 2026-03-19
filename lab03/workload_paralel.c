# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
#include <unistd.h>
#include <sys/wait.h>

#define VECTOR_SIZE 200000000
#define PROCESSOS 4

 // Funcao que simula uma carga de trabalho pesada
void heavy_work ( double * vector , int start , int end) {
	for (int i = start ; i < end ; ++i) {
		vector[i] = sin(vector[i]) * cos(vector[i]) + sqrt(vector[i]);
	}
}

int main () {
    double* vector = (double*) malloc(VECTOR_SIZE * sizeof (double));
    for(int i = 0; i < VECTOR_SIZE; i++) vector [i] = (double)i;

    struct timespec start , end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int chunk_size = VECTOR_SIZE / PROCESSOS;

    for(int i = 0; i < PROCESSOS; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Erro ao criar processo filho\n");
            return 1;
        }
        else if(pid == 0) {

        
            int ini_indice = i * chunk_size;

            int fim_indice = (i == PROCESSOS - 1) ? VECTOR_SIZE : (i + 1) * chunk_size;

            heavy_work(vector, ini_indice, fim_indice);

            free(vector);
            exit(0);
        
        }
    }

    for (int i = 0; i < PROCESSOS; i++) {
        wait(NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_spent = ( end. tv_sec - start . tv_sec ) + (end. tv_nsec - start . tv_nsec ) / 1e9;

    printf ("Versao paralela com %d processos executou em %f segundos \n", PROCESSOS, time_spent);
    // Para verificar se o calculo foi feito , descomente a linha abaixo :
    printf ("Resultado de verificacao no pai: vector [10] = %f\n", vector [10]);

    free(vector);
    return 0;
}