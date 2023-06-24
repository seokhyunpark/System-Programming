#include <stdio.h>

int main() {
	int result = 0;
	for (int i = 1; i <= 500; i++)
		result += i;
	printf("The sum of 1 to 500 is %d", result);

	return 0;
}
