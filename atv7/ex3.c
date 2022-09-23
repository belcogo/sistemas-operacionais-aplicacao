/* primos.c – identifica todos os numeros primos até um certo valor */
/* complile com -lm. Por exemplo: “gcc primo.c -o primo -lm” */
#include<stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/wait.h>
#include <math.h>

struct args {
    short int imprimir;
    long int start;
    long int end;
};

int verifica_se_primo(long int numero);
int thread_function(struct args *arguments);

int main(int argc, char *argv[]) {
    long int numero = 0;
    short int result, imprimir;

    if (argc != 3) {
        printf ("Uso correto: %s <numero> <imprimir=1,nao_imprimir=0>\n\n", argv[0]);
        return 1;
    }

    numero = atol(argv[1]);
    imprimir = atoi(argv[2]);

    clock_t st, ed;
    pthread_t tid1, tid2;
    struct args arguments;

    st = clock();

    arguments.imprimir = imprimir;
    arguments.start = 1;
    arguments.end = numero / 2;

    pthread_create(&tid1, 0, &thread_function, (struct args *)&arguments);

    arguments.start = arguments.end;
    arguments.end = numero;

    pthread_create(&tid2, 0, &thread_function, (struct args *)&arguments);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    ed = clock();
    double time_final = (double)(ed - st) / CLOCKS_PER_SEC;
    printf("%f\n", time_final);

    return 0;
}

int thread_function(struct args *arguments) {
    short int result;
    for (long int num_int = arguments->start; num_int < arguments->end; num_int++) {
        result = verifica_se_primo(num_int);
            if (arguments->imprimir == 1)
                if (result == 1)
                    printf("%ld eh primo.\n", num_int);
    }

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
