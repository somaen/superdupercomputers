#include <math.h>
#include <stdio.h>
#include "PrecisionTimer.h"

int main(
	//int argc, char ** argv
) {
	struct Precision_Timer *pt = calloc(1, sizeof(struct Precision_Timer));
	PT_start(pt);
	size_t limit = 14;
	double *Vector = calloc(1 << limit, sizeof(double));
	// Calculate the vector
	for (int i = 0, j = 1 ; i < (1 << limit) + 0 ; i++, j++) {
		Vector[i] = 1. / j / j;
	}
	PT_stop(pt);
	PT_diffTime(pt);
	char *elapsedTime = calloc(1000, sizeof(char));
	print_timeval(pt, elapsedTime);
	printf("%s\n", elapsedTime);
	for (size_t k = 4 ; k <= limit ; k++) {
		PT_start(pt);
		double acc = 0;
		for (size_t i = 0; i < (1 << k) ; i++) {
			acc += Vector[i];
		}
		printf("Sum of Vector is: %lf\n", acc);
		printf("Error is: %e\n", acc - M_PI * M_PI / 6);
		PT_stop(pt);
		PT_diffTime(pt);
		print_timeval(pt, elapsedTime);
		printf("%s\n", elapsedTime);
	}
	return 0;
}
