/* primos.c – identifica todos os numeros primos ate um certo valor*/
/* complile com -lm. Por exemplo: “gcc primo.c -o primo -lm”*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int verifica_se_primo(long int);
void forker(int init, int end);

int main(int argc, char *argv[]) {
    long int numero = 0;
    short int result;
    numero = atol(argv[1]);
    // nro_processos = atoi(argv[2]);
    // int diff = numero / nro_processos;
    
    
    forker(0, numero);
    
    return 0;
}

int verifica_se_primo(long int numero) {
    long int ant;
    for (ant = 2; ant <= (long int)sqrt(numero); ant++) {
        if (numero%ant == 0)
            return 0;
    }
    if (ant*ant >= numero)
        return 1;
}

void verifica_loop(int inicio, int fim) {
    int i;
    for (i = inicio; i <= fim; i++) {
            verifica_se_primo(i);
    }
}

void forker(int init, int end) {
    int status;
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork");
    }
    else if (pid == 0) {
        int init_c = init;
        int end_c = end / 2;
        verifica_loop(init_c, end_c);
    }
    else {   
        clock_t st, ed;
        st = clock();
        int init_p = (end / 2) + 1;
        int end_p = end;
        verifica_loop(init_p, end_p);
        waitpid(pid, &status, 0);

        ed = clock();
        double time_final = (double)(ed - st) / CLOCKS_PER_SEC;
        printf("%f\n", time_final);
    }
}