#include <math.h>
#include <stdio.h>
#include "PrecisionTimer.h"

int main(
		//int argc, char ** argv
		){
	double *Vector = calloc(1<<14, sizeof(double));
	for ( int i = 0, j=1 ; i < (1<<14)+0 ; i++,j++){
		Vector[i] = 1./j/j;	
	}
	for ( int k = 4 ; k <=14 ; k++ ) {
		double acc = 0;
		for ( int i = 0 ; i < (1<<k) ; i++ ){
			acc+= Vector[i];
		}
		printf("Sum of Vector is: %lf\n", acc);
		printf("Error is: %e\n", acc - M_PI*M_PI/6);
	}
	return 0;
}
