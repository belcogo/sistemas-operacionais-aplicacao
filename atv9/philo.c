#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int idx[N] = {0};

struct result {
    int tried;
    int ate;
};

int state[N], i, ini_rand;
float float_rand;
struct result results[N];

void mostrar(void);
void pensar(int);
void pegar_garfo(int);
void por_garfo(int);
void comer(int);
void test(int);
void *acao_filosofo(void *);

sem_t mutex;
sem_t sem_fil[N];

int main() {
    for (i = 0; i < N; i++) {
        state[i] = 0;
    }

    mostrar();

    int res;
    pthread_t thread[N];
    void *thread_result;

    res = sem_init(&mutex, 0, 1);
    if (res != 0) {
        // ERRO INICIALIZACAO SEMAFORO
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < N; i++) {
        res = sem_init(&sem_fil[i], 0, 0);
        if (res != 0) {
            // ERROR
            exit(EXIT_FAILURE);
        }
        results[i].tried = 0;
        results[i].ate = 0;
    }

    for (i = 0; i < N; i++) {
        res = pthread_create(&thread[i], NULL, &acao_filosofo, &i);
        if (res != 0) {
            // ERROR
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < N; i++) {
        res = pthread_join(thread[i], &thread_result);
        if (res != 0) {
            // ERROR
            exit(EXIT_FAILURE);
        }
    }

    printf("-------------------------------------------------------------------------\nEstatística do jantar dos filósofos\n-------------------------------------------------------------------------\n");

    for(i = 0; i < N; i++) {
        printf("_____________________________________________________________________\nO filósofo %d tentou comer %d vezes, mas comeu %d refeições\n", i, results[i].tried, results[i].ate);
    }

    return 0;
}

void mostrar(void) {
    printf("_____________________________________________________________________\n");
    for(i = 1; i <= N; i++) {
        switch(state[i-1]) {
            case THINKING:
                printf("O filósofo %d está pensando\n", i);
                break;
            case HUNGRY:
                printf("O filósofo %d está com fome\n", i);
                break;
            case EATING:
                printf("O filósofo %d está comendo\n", i);
                break;
        }
    }

    printf("_____________________________________________________________________\n");
}

void pensar(int i) {
    float rand = 0.001*random();
    ini_rand = float_rand;
    usleep(ini_rand);
}

void pegar_garfo(int i) {
    sem_wait(&mutex);
    state[i] = HUNGRY;
    mostrar();
    test(i);
    sem_post(&mutex);
    sem_wait(&sem_fil[i]);
}

void por_garfo(int i) {
    sem_wait(&mutex);
    mostrar();
    test(LEFT);
    test(RIGHT);
    state[i] = THINKING;
    sem_post(&mutex);
}

void comer(int i) {
    float rand = 0.001*random();
    ini_rand = float_rand;
    usleep(ini_rand);
}

void test(int i) {
    results[i].tried += 1;
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        results[i].ate += 1;
        state[i] = EATING;
        mostrar();
        sem_post(&sem_fil[i]);
    }
}

void *acao_filosofo(void *j) {
    int i = *(int *) j;
    while(idx[i] < 10) {
        pensar(i);
        pegar_garfo(i);
        comer(i);
        por_garfo(i);
        idx[i] += 1;
    }
}