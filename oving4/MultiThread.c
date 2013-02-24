#include <omp.h>
#include <math.h>
#include <stdio.h>
#include "PrecisionTimer.h"

int main(
    //int argc, char ** argv
) {
	struct Precision_Timer *pt = calloc(1, sizeof(struct Precision_Timer));
	size_t limit = 29;
	printf("Limit is %zu\n", limit);
	double *Vector = calloc(1 << limit, sizeof(double));
	Vector--;
	// Calculate the vector
	int tid;
	#pragma omp parallel private(tid)
	{
		tid = omp_get_thread_num();
		printf("Hi, I'm a thread, What is love? It is %d!!!!!!!\n", tid);
	}
	PT_start(pt);
	#pragma omp parallel for schedule(static)
	for (int i = 1 ; i < (1 << limit) + 0 ; i++) {
		Vector[i] = 1. / i / i;
	}
	Vector++;
	PT_stop(pt);
	PT_diffTime(pt);
	char *elapsedTime = calloc(1000, sizeof(char));
	print_timeval(pt, elapsedTime);
	printf("%s\n", elapsedTime);
	for (size_t k = 4 ; k <= limit ; k++) {
		PT_start(pt);
		double acc = 0;
		#pragma omp parallel for schedule(static) \
reduction(+:acc)
		for (size_t i = 0; i < (1 << k) ; i++) {
			acc += Vector[i];
		}
		printf("Sum of Vector is: %lf K = %zi\n", acc, k);
		printf("Error is: %e\n", acc - M_PI * M_PI / 6);
		PT_stop(pt);
		PT_diffTime(pt);
		print_timeval(pt, elapsedTime);
		printf("%s\n", elapsedTime);
	}
	return 0;
}
