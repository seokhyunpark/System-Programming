#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include "smsh.h"

#define DFL_PROMPT "> "

int pid, background = 0, background2 = 0;

void setup();
void fatal(char *, char *, int);

int main() {
    char *cmdline, *prompt, **arglist;
    int result;
    void setup();

    prompt = DFL_PROMPT;
    setup();
    while((cmdline = next_cmd(prompt, stdin)) != NULL) {   
        if (strcmp(cmdline, "exit") == 0) {
            if (background2) {
                background = 1;
                background2 = 0;
                kill(pid, SIGINT);
                exit(0);
            }
            else
                exit(0);
        }
        else {
            for (int i = 0; cmdline[i] != '\0'; i++) {
                if (cmdline[i] == '&') {
                    cmdline[i] = '\0';
                    background = 1;
                    break;
                }
            }
            if ((arglist = splitline(cmdline)) != NULL) {
                if (background) {
                    pid = fork();
                    if (pid == -1) {
                        perror("fork");
                    }
                    else if (pid == 0) {
                        signal(SIGINT, SIG_DFL);
                        signal(SIGQUIT, SIG_DFL);
                        execvp(arglist[0], arglist);
                        perror("cannot execute command");
                        exit(1);
                    }
                    else {
                        printf("PID : %d\n", pid);
                        background = 0;
                        background2 = 1;
                    }
                }
                else {
                    result = execute(arglist);
                }
                freelist(arglist);
            }
            free(cmdline);
        }
    }
    return 0;
}

void setup() {
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n) {
    fprintf(stderr, "Error: %s, %s\n", s1, s2);
    exit(n);
}
