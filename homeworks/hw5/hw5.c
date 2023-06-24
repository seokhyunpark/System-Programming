#include <stdio.h>
#include <string.h>
#include <signal.h>

#define LINE "\n=============================================="
#define LINE2 "\n==============================================\n"
#define LINE3 "=============================================="

int cnt, max;

void num_printed(int signum) {
	printf(LINE);
	printf("\nTotal number of printed HelloWorld!: %d", cnt);
	if (max > 0)
		num_remaining();
	printf(LINE2);
	if (cnt % 2 == 1)
		exit(1);
}

void num_remaining() {
	printf("\nNumber of HelloWorld! prints remaining: %d", max - cnt);
}

void print_hello() {
	signal(SIGINT, num_printed);
	while(cnt < max) {
		sleep(1);
		printf("HelloWorld!\n"); cnt++;
	}
	printf(LINE3);
	printf("\nTotal number of printed HelloWorld: %d", cnt);
	num_remaining();
	printf(LINE2);
}

void print_indefinitely() {
	signal(SIGINT, num_printed);
	while (1) {
		sleep(1);
		printf("HelloWorld!\n"); cnt++;
	}
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		max = 0;
		print_indefinitely();
	}
	else if (argc < 3) {
		while (1) {
			printf("Please specify the limit of HelloWorld! to print: ");
			scanf("%d", &max);
			if (max >= 0) {
				print_hello();
				exit(1);
			}
		}
	}
	else if (strcmp(argv[1], "-n") == 0 && atoi(argv[2]) < 0) {
		printf("Invalid input. Please enter a positive integer.\n");
		exit(1);
	}
	else if (strcmp(argv[1], "-n") == 0 && atoi(argv[2]) > 0) {
		max = atoi(argv[2]);
		print_hello();
	}
	return 0;
}
