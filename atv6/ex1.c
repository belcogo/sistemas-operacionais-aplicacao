#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define ECHOMAX 512

void tratador(int signum) {
    char *linha[ECHOMAX];
    printf("Informe a frase a ser apresentada no terminal\n>");
    fgets(linha, ECHOMAX, stdin); 

    char *command[ECHOMAX];
    strcpy(command, "figlet");
    strcat(command, " -c ");
    strcat(command, linha);
    system(command);
}

int main() {
    sigset_t mask;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &tratador;
    pid_t pid = getpid();
    printf("Meu PID: %d envie:\n>kill -USR2 %d para continuar\n>kill -INT %d para sair\n\n", pid, pid, pid);
    sigaction(SIGUSR2, &sa, NULL);

    sigfillset(&mask);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGINT);

    while(1) {
        sigsuspend(&mask);
        sleep(1);
    }

    return 0;
}
