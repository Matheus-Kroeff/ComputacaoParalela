#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define TAMANHO_VETOR 1000
#define NUM_PROCESSOS 4

int fatia_soma(int* vetor, int start, int end) {
    int soma = 0;
    for(int i = start; i < end; i++) {
        soma += vetor[i];
    }
    return soma;
}

void processo_filho(int i, int pipes[NUM_PROCESSOS][2], int* vetor, int fatia) {

    close(pipes[i][0]);

    for (int j = 0; j < NUM_PROCESSOS; j++) {
        if (i != j) {
            close(pipes[j][0]);
            close(pipes[j][1]);
        }
    }

    int start = i * fatia;
    int end = (i == NUM_PROCESSOS - 1) ? TAMANHO_VETOR : (i + 1) * fatia;

    int parte_soma = fatia_soma(vetor, start, end);

    printf("Filho %d, soma calculada: %d\n", getpid(), parte_soma);

    write(pipes[i][1], &parte_soma, sizeof(int));

    close(pipes[i][1]);
    exit(0);
}

int processo_pai(int pipes[NUM_PROCESSOS][2]) {
    int soma_final = 0;

    for (int i = 0; i < NUM_PROCESSOS; i++) {
        close(pipes[i][1]);

        int soma_de_um_filho;
        read(pipes[i][0], &soma_de_um_filho, sizeof(int));

        printf("Soma recebida do filho do pipe %d: %d\n", i, soma_de_um_filho);

        soma_final += soma_de_um_filho;

        close(pipes[i][0]);
    }

    for (int i = 0; i < NUM_PROCESSOS; i++) {
        wait(NULL);
    }

    return soma_final;
}

int main() {

    int vetor[TAMANHO_VETOR];
    int soma_esperada = 0;

    for (int i = 0; i < TAMANHO_VETOR; i++) {
        vetor[i] = i + 1;
        soma_esperada += vetor[i];
    }

    int pipes[NUM_PROCESSOS][2];
    pid_t pid;
    int fatia = TAMANHO_VETOR / NUM_PROCESSOS;

    for (int i = 0; i < NUM_PROCESSOS; i++) {

        if(pipe(pipes[i]) == -1) {
            fprintf(stderr, "Falha na criação do pipe do filho %d\n", i);
            return 1;
        }

        pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork nao funcionou\n");
            return 1;
        }

        if (pid == 0) {
            processo_filho(i, pipes, vetor, fatia);
        }
    }

    int soma_final = processo_pai(pipes);

    printf("Soma final: %d\n", soma_final);
    printf("Soma esperada: %d\n", soma_esperada);

    return 0;
}