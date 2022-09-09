#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    char *message;
    int n;
    int exit_code;

    printf("fork program starting: PID=%d\n", getpid());
    pid = fork();
    switch(pid)
    {
        case -1:
            perror("fork failed\n");
            exit(1);
        case 0:
            message = "child: %d\n";
            n = 5;
            exit_code = 37;
            break;
        default:
            message = "parent: %d\n";
            n = 3;
            exit_code = 0;
            break;
    }

    for(; n > 0; n--) {
        printf(message; getpid());
        sleep(1);
    }

    if (pid != 0) {
        int stat_val;
        pid_t child_pid;

        child_pid = wait(&stat_val);

        printf("parent terminated\n");
        if (WIFEXITED(stat_val)) {
            printf("msg from parent: child %d exited with code %d\n", child_pid, WEXITSTATUS(stat_val));
        } else {
            printf("msg from parent: child %d terminated abnormally\n", child_pid);
        }
    } else {
        printf("child has terminated\n");
    }

    exit(exit_code);
}