/* primos.c – identifica todos os numeros primos ate um certo valor*/
/* complile com -lm. Por exemplo: “gcc primo.c -o primo -lm”*/

#include<stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

int verifica_se_primo(long int);
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
    st = clock();
    for (long int num_int = 1; num_int <= numero; num_int++) {
        result = verifica_se_primo(num_int);
        if (imprimir == 1)
            if (result == 1)
                printf("%ld eh primo.\n", num_int);
    }
    ed = clock();
    double time_final = (double)(ed - st) / CLOCKS_PER_SEC;
    printf("%f\n", time_final);
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