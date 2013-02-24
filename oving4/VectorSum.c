#include <stdio.h>
#include <math.h>
#include "mpi.com.h" //Trivial wrappers for some MPI functions, 
//also giving the uplink global struct which contains nprocs 
//and rank

double *generateVector(size_t binaryLogLength);

int main(int argc, char **argv) {
	mpi_com_Init(&argc, & argv);
	double *Vector = NULL;
	double *recvb = NULL;
	for (size_t k = 4 ; k <= 24 ; k++) {
		size_t range = (1 << (k + 1))/(uplink.nprocs);
		Vector = generateVector(k);
		Vector[0] = reducePlus(Vector, (int)range);
		if (uplink.rank == 0) {
			recvb = realloc(recvb, (size_t)range * sizeof(double));
			MPI_Reduce((void *)Vector, (void *)recvb, (int)range, 
					MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
			printf("Error is: %e, k=%zu\n", 
					M_PI*M_PI/6 - recvb[0],k );
		} else {
			recvb = NULL;
			MPI_Reduce((void *)Vector, (void *)recvb, (int)range, 
					MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		}
	}
	free(Vector);
	free(recvb);
	mpi_com_Finalize();
	return 0;
}

double *generateVector(size_t binaryLogLength) {
	size_t range = (1 << (binaryLogLength + 1))/(uplink.nprocs);
	size_t lowLim = range * uplink.rank;
	size_t highLim = lowLim + range;
	double *Vector = realloc(NULL, range * sizeof(double));
	for (size_t i = lowLim ; i < highLim ; i ++) {
		Vector[i - lowLim] = pow(i + 1, -2);
	}
	return Vector;
}

