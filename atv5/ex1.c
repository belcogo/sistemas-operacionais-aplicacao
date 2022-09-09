#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ECHOMAX 1048576
#define ECHOMIN 512
#define FILENAME "resultfile"

void execute_command(char command[ECHOMIN], int file_descriptor[2]);
char *prepare_command(char command[ECHOMIN], char *args[], int argsize);
char *get_result(char result[ECHOMAX]);

int main(int argc, char *argv[]) {
    int file_pipes[2];
    pid_t fork_result;

    if (pipe(file_pipes) == 0) {
        fork_result = fork();

        if (fork_result > 0) {
            //parent
            close(file_pipes[0]);
            char curl_command[ECHOMIN];
            char *curl_args[] = {"curl ", argv[1], " -o ", FILENAME, " --silent"};
            char complete_command[ECHOMIN];
            char *args[] = {"curl ", argv[1], " -o ", FILENAME, " --silent | egrep -oi '", argv[2], "' | wc -l"};
            prepare_command(complete_command, args, 7);
            prepare_command(curl_command, curl_args, 5);
            printf(">%s\n", complete_command);
            execute_command(curl_command, file_pipes);
            close(file_pipes[1]);
        }

        if (fork_result == 0) {
            //child
            char egrep_command[ECHOMIN];
            char *egrep_args[] = {"egrep -oi '", argv[2], "' | wc -l"};
            prepare_command(egrep_command, egrep_args, 3);

            close(0);
            close(file_pipes[1]);
            int new_stdin = dup(file_pipes[0]);

            system(egrep_command);
            
            close(file_pipes[0]);
            close(new_stdin);

            char erase_file_command[ECHOMIN];
            char *erase_args[] = {"rm ", FILENAME};
            prepare_command(erase_file_command, erase_args, 2);
            system(erase_file_command);
        }
    }

    return 0;
}

void execute_command(char command[ECHOMIN], int file_descriptor[2]) {
    char result[ECHOMAX];

    system(command);
    get_result(result);
    write(file_descriptor[1], result, strlen(result));
}

char *prepare_command(char command[ECHOMIN], char *args[], int argsize) {
    strcpy(command, args[0]);

    int i = 1;
    while(i < argsize) {
        strcat(command, args[i]);
        i++;
    }

    return command;
}

char *get_result(char result[ECHOMAX]) {
	FILE *fp;
	char buffer[ECHOMAX];
	fp = fopen(FILENAME, "r");
	int i = 0;

	while (i < ECHOMAX) {
        buffer[i] = fgetc(fp);

        if (buffer[i] == EOF) {
            break;
        }
		++i;
	}

	fclose(fp);
    strcpy(result, buffer);

	return result;
}