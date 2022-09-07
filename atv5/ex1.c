#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ECHOMAX 1024

void aaa(char command[ECHOMAX], int file_descriptor[2]);

char *get_result(char result[ECHOMAX]);

int main() {
    int data_processed;
    int file_pipes[2];
    pid_t fork_result;

    if (pipe(file_pipes) == 0) {
        fork_result = fork();
        // if (fork_result == -1) {
        //     fprintf(stderr, "Fork failure");
        //     exit(EXIT_FAILURE);
        // }

        printf("%d\n", fork_result);

        if (fork_result > 0) {
            printf("Oi 3");
            close(file_pipes[0]);
            aaa("curl https://g1.globo.com -o a", file_pipes);
            close(file_pipes[1]);
        }

        if (fork_result == 0) {
            //child
            close(0);
            close(file_pipes[1]);
            dup(file_pipes[0]);
            printf("Oi\n");
            char *readBuffer[ECHOMAX];
            ssize_t n = read(file_pipes[0], readBuffer, sizeof(readBuffer));
            readBuffer[n] = 0;
            close(file_pipes[0]);
            if (n > 0) {
                printf("%s", readBuffer);
            }
            // system("rm a.txt");
        }
    }

    return 0;
}

void aaa(char command[ECHOMAX], int file_descriptor[2]) {
    printf("Oi 2");
    char result[ECHOMAX];
    system(command);
    get_result(result);
    // write(file_descriptor[1], result, strlen(result));
}

char *get_result(char result[ECHOMAX]) {
	FILE *fp;
	char buffer[ECHOMAX];
    // char aaa[ECHOMAX];
    
	fp = fopen("a", "r");
	// int i = 0;
	// while (1) {
    //     // printf("%s\n", fgetc(fp));
	// 	// buffer[i] = fgetc(fp); // reading the file
	// 	if (buffer[i] == EOF) {
    //         break;
    //     }
	// 	++i;
	// }
    fgets(buffer, ECHOMAX, fp);
	fclose(fp);
    strcpy(result, buffer);
	return result;
}