#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ECHOMAX 5120

void aaa(char command[ECHOMAX], int file_descriptor[2]);

char *get_result(char result[ECHOMAX]);

int main() {
    int data_processed;
    int file_pipes[2];
    pid_t fork_result;

    if (pipe(file_pipes) == 0) {
        fork_result = fork();

        // printf("%d\n", fork_result);

        if (fork_result > 0) {
            close(file_pipes[0]);
            aaa("curl https://g1.globo.com -o a --silent", file_pipes);
            close(file_pipes[1]);
        }

        if (fork_result == 0) {
            //child
            close(0);
            close(file_pipes[1]);
            dup(file_pipes[0]);
            printf("> grep -oi 'coronavirus' | wc -l\n");
            // system("cat a");
            system("grep -oi 'coronavirus' | wc -l");
            // char *readBuffer[ECHOMAX];
            // ssize_t n = read(file_pipes[0], readBuffer, sizeof(readBuffer));
            // readBuffer[n] = 0;
            close(file_pipes[0]);
            // if (n > 0) {
            //     printf("%s", readBuffer);
            // }
            // system("rm a");
        }
    }

    return 0;
}

void aaa(char command[ECHOMAX], int file_descriptor[2]) {
    char result[ECHOMAX];
    system(command);
    get_result(result);
    write(file_descriptor[1], result, strlen(result));
}

char *get_result(char result[ECHOMAX]) {
	FILE *fp;
	char buffer[ECHOMAX];
	fp = fopen("a", "r");
	int i = 0;
	while (i) {
		fgets(buffer[i], sizeof(fp), fp);
        if (buffer[i] == EOF) {
            break;
        }
        // printf("%s\n", buffer[i]);
		++i;
	}
    // fgets(buffer, sizeof(fp), fp);
	// fclose(fp);
    strcat(buffer, "\n");
    strcpy(result, buffer);
    printf("OLHA AQUI>>>>>>>>>>>>>> \n%s\n", buffer);
	return result;
}