// Execute grep | ls
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char* grep = "grep";
    char* ls = "ls";
    char* c = "\\.c$";

    int pid;
    int thepipe[2];

    // Create a pipe
    if (pipe(thepipe) == -1) {
        perror("pipe");
        exit(1);
    }

    // Create a child process
    if ((pid = fork()) == -1) {
        perror("fork");
        exit(1);
    }
    // Create a parent process
    if (pid > 0) {
        close(thepipe[1]);
        dup2(thepipe[0], 0);
        close(thepipe[0]);
        execlp(grep, grep, c, NULL);

        perror("execlp");
        exit(1);
    }
    close(thepipe[0]);
    dup2(thepipe[1], 1);
    close(thepipe[1]);
    execlp(ls, ls, NULL);

    perror("execlp");
    exit(1);
}
