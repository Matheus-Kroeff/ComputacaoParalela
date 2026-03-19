#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int N = 3; 

    for(int i = 0; i < N; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork falhou!\n");
            return 1;
        } else if (pid == 0) {
            // Código do Filho
            printf("Filho: PID %d, meu pai e pai %d\n", getpid(), getppid());
        } else {
            // Código do Pai
            printf("Pai: PID %d, criou o filho %d\n", getpid(), pid);
            wait(NULL); 
            printf("Filho terminou, pai saindo do loop.\n");
            //break; 
        }
    }
    return 0;
}
