#include <math.h>
#include <stdio.h>
#include "PrecisionTimer.h"

int main(
	//int argc, char ** argv
) {
	struct Precision_Timer *pt = calloc(1, sizeof(struct Precision_Timer));
	PT_start(pt);
	double *Vector = calloc(1 << 14, sizeof(double));
	// Calculate the vector
	for (int i = 0, j = 1 ; i < (1 << 14) + 0 ; i++, j++) {
		Vector[i] = 1. / j / j;
	}
	PT_stop(pt);
	char *elapsedTime = calloc(1000, sizeof(char));
	print_timeval(pt, elapsedTime);
	printf("%s\n", elapsedTime);
	int start = 0;  // Avoid recalculation in the summation
	double acc = 0;
	for (int k = 4 ; k <= 14 ; k++) {
		PT_start(pt);
		int i = start;
		for (; i < (1 << k) ; i++) {
			acc += Vector[i];
		}
		start = i;
		printf("Sum of Vector is: %lf\n", acc);
		printf("Error is: %e\n", acc - M_PI * M_PI / 6);
		print_timeval(pt, elapsedTime);
		printf("%s\n", elapsedTime);
	}
	return 0;
}
