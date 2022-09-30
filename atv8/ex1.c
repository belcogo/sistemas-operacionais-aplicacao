

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 10;
int sum = 0;
int index = 0;
float medias[10];
int current_media = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condicao_calculate_media = PTHREAD_COND_INITIALIZER;
pthread_cond_t condicao_calculate_medias_media = PTHREAD_COND_INITIALIZER;

void espera();
void* thread_C();
void* thread_P();
void* thread_M();

void espera() {
    usleep((useconds_t) 1);
}

void* thread_C() {
    while(1) {
        pthread_mutex_lock(&mutex);
        for (index = 0; index < 10; index++) {
            int valor = random() % 5000;
            sum += valor;
            printf("C| %d |\n", sum);
            espera();
        }
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condicao_calculate_media);
        break;
    }
}

void* thread_P() {
    while(1) {
        pthread_mutex_lock(&mutex);
        while(index != 10) {
            pthread_cond_wait(&condicao_calculate_media, &mutex);
        }
        medias[current_media] = sum / 10;
        printf("P| %f |\n", medias[current_media]);
        sum = 0;
        current_media++;
        pthread_mutex_unlock(&mutex);
        if (current_media == 9) {
            pthread_cond_signal(&condicao_calculate_medias_media);
            break;
        }
    }
}

void* thread_M() {
    while(1) {
        pthread_mutex_lock(&mutex);
        while( current_media != 9) {
            pthread_cond_wait(&condicao_calculate_medias_media, &mutex);
        }
        float medias_media;
        float medias_sum = 0;
        for (int i = 0; i < current_media; i++) {
            medias_sum += medias[i];
        }
        medias_media = medias_sum / current_media;
        printf("M| %f |\n", medias_media);
        for (int i = 0; i < current_media; i++) {
            if (medias[i] < medias_media) {
                printf("Media nro #%d = %f\nMedia geral = %f\n", i, medias[i], medias_media);
            }
        }
        pthread_mutex_unlock(&mutex);
        break;
    }
}

int main() {
    pthread_t threads[3];
    srand(time(NULL));
    pthread_create(&threads[0], NULL, &thread_C, NULL);
    pthread_create(&threads[1], NULL, &thread_P, NULL);
    pthread_create(&threads[2], NULL, &thread_M, NULL);

    pthread_exit(NULL);
    return 0;
}