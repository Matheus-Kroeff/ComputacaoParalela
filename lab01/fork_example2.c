#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int N = 4; 

    for(int i = 0; i < N; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork falhou!\n");
            return 1;
        } else if (pid == 0) {
            // Código do Filho
            printf("[Filho %d] PID: %d, Pai: %d\n", i + 1, getpid(), getppid());
            return 0;
        } else {
            // Código do Pai
            printf("Pai: PID %d, criou o filho %d\n", getpid(), pid);
            //break; 
        }
    }
    for(int i = 0; i < N; i++) {
        wait(NULL);
    }
    
    printf("Todos os filhos terminaram. Pai finalizando.\n");
    return 0;
}


