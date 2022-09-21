#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#define ECHOMAX 512
#define THREADS_TO_CREATE 3
#define DEFAULT_SEPARATOR "_________________________"

int *thread_creator(int thread_to_create);
int print_message(char *message[ECHOMAX]);
int print_message_sleep(char *message[ECHOMAX]);

int main() {
    int threads_to_create = 0;
    printf("\n%s\n", DEFAULT_SEPARATOR);
    while (threads_to_create < THREADS_TO_CREATE) {
        thread_creator(threads_to_create);
        threads_to_create++;
    }
    printf("\n%s\n", DEFAULT_SEPARATOR);
}

int *thread_creator(int thread_to_create) {
    char *message[ECHOMAX];
    char *messages[3]= {"AAAAA", "BBBBB", "CCCCC"};
    pthread_t pid;

    strcpy(message, messages[thread_to_create]);

    if (thread_to_create = 1) {
        pthread_create(&pid, 0, &print_message_sleep, &message);
    } else {
        pthread_create(&pid, 0, &print_message, &message);
    }
    pthread_join(pid, NULL);

    return 0;
}

int print_message(char *message[ECHOMAX]) {
    printf(message);
}

int print_message_sleep(char *message[ECHOMAX]) {
    sleep(2);
    printf(message);
}
