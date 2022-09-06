#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ECHOMAX 1024

char *execute_command(char *command[ECHOMAX]);

int main(int argc, char *argv[]) {
    int data_processed;
    int file_pipes[2];
    pid_t fork_result;

    if (pipe(file_pipes) == 0) {
        fork_result = fork();
        if (fork_result == -1) {
            fprintf(stderr, "Fork failure");
            exit(EXIT_FAILURE);
        }

        if (fork_result == 0) {
            //child
            close(0);
            close(file_pipes[1]);
            dup(file_pipes[0]);
            char *readBuffer[ECHOMAX];
            // ssize_t n = read(file_pipes[0], readBuffer, sizeof(readBuffer));
            // readBuffer[n] = 0;
            close(file_pipes[0]);
            // if (n > 0) {
                // printf("%s", readBuffer);
            // }
            // system("rm a.txt");
        } else {
            //parent
            close(file_pipes[0]);
            char *result = execute_command("curl https://g1.globo.com");
            // write(file_pipes[1], result, strlen(result));
            close(file_pipes[1]);
            
        }
    }

    return 0;
}

char *execute_command(char *command[ECHOMAX]) {
	FILE *fp;
	char *buffer[ECHOMAX];
    // system(command);
	fp = popen(command, "r");
	int i = 0;
	while (1)
	{
		buffer[i] = fgetc(fp); // reading the file
		if (buffer[i] == EOF) break;
		++i;
	}
	pclose(fp);
		
	return buffer;
}