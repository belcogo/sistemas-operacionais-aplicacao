#include <unistd.h>
// #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define message "Recebido sinal %s (%d)\n"

void tratador(int signum) {
    switch (signum) {
        case SIGUSR1: printf(message, "SIGUSR1", signum); break;
        case SIGCONT: printf(message, "SIGCONT", signum); break;
        case SIGTERM: printf(message, "SIGTERM", signum); break;
        case SIGINT: printf(message, "SIGINT", signum); break;
        case SIGTSTP: printf(message, "SIGTSTP", signum); break;
        case SIGQUIT: printf(message, "SIGQUIT", signum); break;
    }
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &tratador;
    printf("Meu PID: %d\n", getpid());
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGCONT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    for(;;) {}

    return 0;
}