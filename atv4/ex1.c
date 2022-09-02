#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define TRUE 1
#define ECHOMAX 512

int main() {
	char delimit[] = " ";
	int status;

	while (TRUE) {
		char *linha[ECHOMAX];
		gets(linha);

		char *command = strtok(linha, delimit);
		char *conf = strtok(NULL, delimit);
		char *prog[] = {"/bin/"};
		char *aux[ECHOMAX];

		strcpy(aux, prog[0]);
		strcat(aux, command);

		char *parameters[] = {aux, conf, "/", NULL};
		
		if (fork() != 0) {
			waitpid(-1, &status, 0);
		} else {
			execve(aux, parameters, NULL);
		}
	}

	return 0;
}
