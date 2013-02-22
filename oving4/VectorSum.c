#include <sys/time.h>
#include <stdio.h>
#include "PrecisionTimer.h"
#include "mpi.com.h"
#include <math.h>
double *generateVector(size_t binaryLogLength);

int main(int argc, char **argv){
	mpi_com_Init(&argc, & argv);
#ifdef DEBUG
	printf( "rank=%ld\n", uplink.rank); 
	printf( "procs=%ld\n", uplink.nprocs) ;
#endif
	double *Vector =NULL;
	double *recvb = NULL;
	for ( size_t k = 4 ; k <=14 ; k++ ) {
		int range =1<<(k-uplink.nprocs+1); 
		Vector = generateVector( k );
		Vector[0] =reducePlus(Vector, range);
		if (uplink.rank == 0){
			recvb = realloc(recvb, (size_t)range* sizeof(double));
			MPI_Reduce( (void*)Vector, (void*)recvb, range, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
			printf("Sum: %lf\n", recvb[0]);
			printf("Error is: %e\n", recvb[0] - M_PI*M_PI/6);
		}else {
			recvb = NULL;
			MPI_Reduce( (void*)Vector, (void*)recvb, range, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
		}
	}
	free(Vector );
	free(recvb );
	mpi_com_Finalize();
	return 0;
}

double *generateVector( size_t binaryLogLength) {
	size_t range =1<<(binaryLogLength-uplink.nprocs+1); 
	size_t lowLim = range*uplink.rank;
	size_t highLim = lowLim+range;
	double *Vector = realloc(NULL,range* sizeof(double));
	for ( size_t i = lowLim ; i < highLim ; i ++){
		Vector[i-lowLim] = pow(i+1,-2);
	}
	return Vector;
}

