#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int status;
    long nroProcessos = strtol(argv[1], NULL, 10);
    int i;
    if (nroProcessos == 0) {
        printf("Meu ID: %d -- ID do meu pai: %d\n", getpid(), getppid());
    } else {
        printf("Meu ID: %d\n", getpid());
    }
    while (i < nroProcessos) {
        if (fork() == 0) {
            execl("./exp3", "./exp3", "0");
        }
        i++;
    }
    return 0;
}