/**
 * ELEC462: HW#1 - gdb Exercise
 * School of Computer Science & Engineering
 * Kyungpook National University
 *
 * File name: factorization_org.c
 * Writer: Prof. Y.-K. Suh 
 */

#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]){
	int i_a, i_b, i_c;
	double alpha, beta, d_D;

	printf("aX^2 + bX + c = 0\n");

	printf("a: ");
	scanf("%d", &i_a);

	printf("b: ");
	scanf("%d", &i_b);

	printf("c: ");
	scanf("%d", &i_c);

	d_D = i_b*i_b - 4*i_a*i_c;
	printf("%f\n", d_D);
	if(d_D < 0){
		alpha = ((-1 * i_b) + sqrt(d_D)) / (double)(2 * i_a);
		beta = ((-1 * i_b) - sqrt(d_D)) / (double)(2 * i_a);
		printf("%d(x%+.2f)(x%+.2f) = 0\n",i_a, -1 * alpha, -1 * beta);
	}
	else
		printf("Doesn't make sense\n");
	return 0;
}
