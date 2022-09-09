#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

volatile sig_atomic_t counter = 0;

void tratador(int signum) {
    counter++;
}

int main() {
    sigset_t mask;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &tratador;
    printf("Meu PID: %d\nEnvie USR1 para continuar ou INT para sair\n", getpid());
    sigaction(SIGUSR1, &sa, NULL);

    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);

    while(1) {
        sigsuspend(&mask);
        sleep(1);
        printf("Contador: %d\n", counter);
    }

    return 0;
}