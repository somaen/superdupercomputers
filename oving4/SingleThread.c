#include <math.h>
#include <stdio.h>
#include "PrecisionTimer.h"

int main() {
	char *elapsedTime = calloc(1000, sizeof(char));
	struct Precision_Timer *pt = calloc(1,
			sizeof(struct Precision_Timer));
	PT_start(pt);
	size_t limit = 14;
	printf("Limit is %zu\n", limit);
	double *Vector = calloc(1 << limit, sizeof(double));
	size_t ulim = 1 << limit;
	for (size_t i = 0, j = 1 ; i < ulim ; i++, j++) {
		Vector[i] = 1. / j / j;
	}
	PT_stop(pt);
	PT_diffTime(pt);
	print_timeval(pt, elapsedTime);
	printf("%s\n", elapsedTime);
	for (size_t k = 4 ; k <= limit ; k++) {
		PT_start(pt);
		double acc = 0;
		ulim = 1 << k;
		for (size_t i = 0; i < ulim ; i++) {
			acc += Vector[i];
		}
		printf("Sum - pi²/6: %e\n", acc - M_PI*M_PI/6);
		PT_stop(pt);
		PT_diffTime(pt);
		print_timeval(pt, elapsedTime);
		printf("%s\n", elapsedTime);
	}
	return 0;
}
