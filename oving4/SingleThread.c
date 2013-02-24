#include <math.h>
#include <stdio.h>

int main() {
	char *elapsedTime = calloc(1000, sizeof(char));
	struct Precision_Timer *pt = calloc(1,
			sizeof(struct Precision_Timer));
	size_t limit = 14;
	double *Vector = calloc(1 << limit, sizeof(double));
	size_t ulim = 1 << limit;
	for (size_t i = 0, j = 1 ; i < ulim ; i++, j++) {
		Vector[i] = 1. / j / j;
	}
	for (size_t k = 4 ; k <= limit ; k++) {
		double acc = 0;
		ulim = 1 << k;
		for (size_t i = 0; i < ulim ; i++) {
			acc += Vector[i];
		}
		printf("Sum - pi²/6: %e\n", acc - M_PI*M_PI/6);
		print_timeval(pt, elapsedTime);
		printf("%s\n", elapsedTime);
	}
	return 0;
}
