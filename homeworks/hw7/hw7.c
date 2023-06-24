#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int ifOdd(int num) {
    int temp, result = 1;

    while (num != 0) {
        temp = num % 10;
        if (temp != 0)
            result = result * temp;
        num /= 10;
    }
    return result;
}

int ifEven(int num) {
    int temp, result = 0;

    while (num != 0) {
        temp = num % 10;
        result += temp;
        num /= 10;
    }
    return result;
}

int main(int argc, char *argv[]) {
    int num, pid, ret_from_fork, result;
    printf("Enter the number of child processes: ");
    scanf("%d", &num);

    for (int i = 1; i <= num; i++) {
        ret_from_fork = fork();
        if (ret_from_fork == -1) {
            perror("fork");
            exit(0);
        }
        else if (ret_from_fork == 0) {
            pid = getpid();
            if (pid % 2 == 0) {
                result = ifEven(pid);
                printf("Child Process %d [PID - %d]: result(even) = %d\n", i, pid, result);
            }
            else {
                result = ifOdd(pid);
                printf("Child Process %d [PID - %d]: result(odd) = %d\n", i, pid, result);
            }
            exit(0);
        }
        else {
            wait(NULL);
        }
    }
    return 0;
}
