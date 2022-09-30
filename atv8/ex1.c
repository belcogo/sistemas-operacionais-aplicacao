

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int sum = 0;
int current_media = 0;
int idx = 0;
int linear = 0;
float medias[6];
float sums[10];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condicao_calculate_media = PTHREAD_COND_INITIALIZER;
pthread_cond_t condicao_calculate_medias_media = PTHREAD_COND_INITIALIZER;

void espera();
void* thread_C();
void* thread_P();
void* thread_M();

void espera() {
    sleep(1);
}

void* thread_C() {
    while(1) {
        pthread_mutex_lock(&mutex);
        for (int k = 0; k < 6; k++) {
            sums[k] = 0;
            for (int i = 0; i < 10; i++) {
                int valor = random() % 5000;
                sums[k] += valor;
                espera();
            }
        }
        idx = 1;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condicao_calculate_media);
        break;
    }
}

void* thread_P() {
    while(1) {
        pthread_mutex_lock(&mutex);
        while(idx == 0) {
            pthread_cond_wait(&condicao_calculate_media, &mutex);
        }
        for (int i = 0; i < 6; i++) {
            medias[current_media] = sums[current_media] / 10;
            current_media++;
        }
        pthread_mutex_unlock(&mutex);
        break;
    }
    pthread_cond_signal(&condicao_calculate_medias_media);
}

void* thread_M() {
    while(1) {
        pthread_mutex_lock(&mutex);
        while(current_media < 1) {
            pthread_cond_wait(&condicao_calculate_medias_media, &mutex);
        }
        printf("\n_______________________________\n*******************************\nLINEAR = %d\n_______________________________\nMÉDIAS ABAIXO DA MÉDIA GERAL\n", linear);
        for (int i = 0; i < current_media; i++) {
            if (medias[i] < linear) {
                printf("Media nro #%d = %f\n", i, medias[i]);
            }
        }
        printf("_______________________________");
        pthread_mutex_unlock(&mutex);
        break;
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf ("Uso correto: %s <linear>\n\n", argv[0]);
        return 1;
    }

    pthread_mutex_lock(&mutex);
    linear = atol(argv[1]);
    pthread_mutex_unlock(&mutex);

    pthread_t tids[3];
    srand(time(NULL));

    printf("MÉDIA DE TEMPERATURAS MEDIDAS DURANTE 1MINUTO\n");

    pthread_create(&tids[0], NULL, &thread_C, NULL);
    pthread_create(&tids[1], NULL, &thread_P, NULL);
    pthread_create(&tids[1], NULL, &thread_M, NULL);

    pthread_exit(NULL);
    return 0;
}