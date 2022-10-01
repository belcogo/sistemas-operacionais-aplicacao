#include <stdiu.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

sem_t sem_p1;
sem_t sem_p2;

void espera_tempo_aleatorio() {
    useconds_t tempo;
    tempo = random() % 5000;
    usleep(tempo);
}

void* processo_p1() {
    espera_tempo_aleatorio();
    printf("Início P1");
    espera_tempo_aleatorio();
    printf("Fim P1");
    sem_post(&sem_p1);
    return NULL;
}

void* processo_p2() {
    espera_tempo_aleatorio();
    sem_wait(&sem_p1);
    printf("Início P2");
    espera_tempo_aleatorio();
    printf("Fim P2");
    sem_post(&sem_p2);
    return NULL;
}

void* processo_p3() {
    espera_tempo_aleatorio();
    sem_wait(&sem_p2);
    printf("Início P3");
    espera_tempo_aleatorio();
    printf("Fim P3");
    return NULL;
}

void* processo_p4() {
    espera_tempo_aleatorio();
    printf("Início P4");
    espera_tempo_aleatorio();
    printf("Fim P4");
    return NULL;
}

int main() {
    pthread_t threads[4];
    srand(time(NULL));
    sem_init(&sem_p1, 0, 0);
    sem_init(&sem_p2, 0, 0);

    pthread_create(&threads[0], NULL, &processo_p1, NULL);
    pthread_create(&threads[1], NULL, &processo_p2, NULL);
    pthread_create(&threads[2], NULL, &processo_p3, NULL);
    pthread_create(&threads[3], NULL, &processo_p4, NULL);

    pthread_exit(NULL);
    return 0;
}
