#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

void forker(int n);

int main(int argc, char *argv[]) {
    long qtd_processes = strtol(argv[1], NULL, 10);

    printf("Meu ID: %d\n", getpid());
    forker(qtd_processes);
    return 0;
}

// fonte: https://stackoverflow.com/questions/876605/multiple-child-process
void forker(int n) {
    pid_t pid;

    if(n > 0) {
        if ((pid = fork()) < 0) {
            perror("fork");
        }
        else if (pid == 0) {
            printf("Meu ID: %d -- ID do meu pai: %d\n", getpid(), getppid());
        }
        else if(pid > 0) {
            forker(n - 1);
        }
    }
}