#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 100

FILE *popen2(const char*, const char*);
void sigint_handler(int);
void sigquit_handler(int);
void sigterm_handler(int);

int main() {
    FILE *pipein_fp, *pipeout_fp;
    char readbuf[BUFF_SIZE];


    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    signal(SIGTERM, sigterm_handler);

    if ((pipein_fp = popen2("ls", "r")) == NULL) {
        perror("popen2");
        return 1;
    }
    if ((pipeout_fp = popen2("sort -r", "w")) == NULL) {
        perror("popen2");
        return 1;
    }

    printf("PID: %d\n", getpid());
    for (int i = 0; i < 5; i++) {
        sleep(1);
    }

    while (fgets(readbuf, BUFF_SIZE, pipein_fp)) {
        fputs(readbuf, pipeout_fp);
    }
    pclose(pipein_fp);
    pclose(pipeout_fp);
    for (int i = 0; i < 10; i++) {
        sleep(1);
    }
    return 0;
}

FILE *popen2(const char *command, const char *mode) {
    int fd[2];
    FILE *fp;
    pid_t pid;

    if (pipe(fd) == -1) {
        perror("pipe");
        return NULL;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return NULL;
    }
    if (pid > 0) {
        signal(SIGINT, sigint_handler);
        signal(SIGQUIT, sigquit_handler);
        signal(SIGTERM, sigterm_handler);

        if (mode[0] == 'r') {
            close(fd[1]);
            fp = fdopen(fd[0], "r");
        }
        else if (mode[0] == 'w') {
            close(fd[0]);
            fp = fdopen(fd[1], "w");
        }
    }
    else {
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTERM, SIG_IGN);

        if (mode[0] == 'r') {
            close(fd[0]);
            dup2(fd[1], 1);
        }
        else if (mode[0] == 'w') {
            close(fd[1]);
            dup2(fd[0], 0);
        }
        execl("/bin/sh", "sh", "-c", command, NULL);
        exit(1);
    }
    return fp;
}

void sigint_handler(int sig) {
    signal(sig, SIG_IGN);
    printf("\tSIGINT received but ignored…\n");
}

void sigquit_handler(int sig) {
    signal(sig, SIG_IGN);
    printf("\tSIGQUIT received but ignored…\n");
}

void sigterm_handler(int sig) {
    signal(sig, SIG_IGN);
    printf("\tSIGTERM received but ignored…\n");
}
