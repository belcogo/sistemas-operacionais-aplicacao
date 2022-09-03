#include "stdlib.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

#define status 0

void createFile(int range_init, int range_end, bool is_ascii);
void forker(int n);

int main() {
    FILE *fp;
    char c;
    forker(2);

    fp = fopen("numbers.txt", "r");
    while ((c = getc(fp)) != EOF) {
        printf("%c", c);
    }

    fclose(fp);

    fp = fopen("alphabet.txt", "r");
    while ((c = getc(fp)) != EOF) {
        printf("%c", c);
    }

    fclose(fp);

    return 0;
}

void createFile(int range_init, int range_end, bool is_ascii) {
    FILE *fp;
    char *fileName = "numbers.txt";

    if (is_ascii) {
        fileName = "alphabet.txt";
    }

    fp = fopen(fileName, "w");

    for (int i = range_init; i <= range_end; i++) {
        char *str;
        char str_aux[1];
        
        if (is_ascii) {
            sprintf(str_aux, "%c", i);
        } else {
            sprintf(str_aux, "%d", i);
        }

        str = str_aux;

        strcat(str, "\n");
        fputs(str, fp);
        
    }

    fclose(fp);
}

void forker(int n) {
    if(n > 0) {
        pid_t pid;

        int range_init = 1;
        int range_end = 10;
        bool is_ascii = false;

        if (n == 2) {
            range_init = 65;
            range_end = 90;
            is_ascii = true;
        }

        if ((pid = fork()) < 0) {
            perror("fork");
        }
        else if (pid == 0) {
            createFile(range_init, range_end, is_ascii);
        }
        else if(pid > 0) {
            waitpid(pid, status, 0);
            forker(n - 1);
        }
    }
}