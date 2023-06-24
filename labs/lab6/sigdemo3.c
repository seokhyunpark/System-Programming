#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

time_t start;

void f(int signum) {
	time_t end = time(NULL);
	printf("Currently elapsed time: %d sec(s)", end - start);
}

int main() {
	start = time(NULL);

	signal(SIGINT, f);
	printf("you can't stop me!\n");
	while(1) {
		sleep(1);
		printf("haha\n");
	}

}
