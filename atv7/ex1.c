#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#define ECHOMAX 512
#define ESPECIAL_SEPARATOR "**************************************************"
#define DEFAULT_SEPARATOR "_________________________"

struct args {
    int no_of_threads;
    pthread_t pid;
};

int *thread_creator(struct args *arguments);
void print_message(char *message[ECHOMAX]);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_message("USAGE:\n> ./[process_name] <number_of_threads_to_create>");
        exit(0);
    }

    long number_of_threads = strtol(argv[1], NULL, 10);
    pthread_t pid;
    struct args arguments;

    arguments.no_of_threads = number_of_threads - 1;
    arguments.pid = gettid();

    pthread_create(&pid, 0, &thread_creator, (struct args *)&arguments); 
    pthread_join(pid, NULL);

    printf("%s\n\n> meu TID - %d\n%s\n", DEFAULT_SEPARATOR, gettid(), DEFAULT_SEPARATOR);

    return 0;
}

int *thread_creator(struct args *arguments) {
    if (arguments->no_of_threads <= 0) {
        return 0;      
    }

    pthread_t tid;
    struct args new_arguments;

    new_arguments.no_of_threads = arguments->no_of_threads - 1;
    new_arguments.pid = gettid();

    pthread_create(&tid, 0, &thread_creator, (struct args *)&new_arguments);
    pthread_join(tid, NULL);

    printf("%s\n\n> parent TID - %d\n> meu TID - %d\n%s\n", DEFAULT_SEPARATOR, arguments->pid, gettid(), DEFAULT_SEPARATOR);

    return 0;
}

void print_message(char *message[ECHOMAX]) {
    printf("\n%s\n%s\n%s%s\n%s\n", ESPECIAL_SEPARATOR, message, DEFAULT_SEPARATOR, DEFAULT_SEPARATOR, ESPECIAL_SEPARATOR);
}