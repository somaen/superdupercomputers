#include <stdio.h>
#include "mpi.com.h"
#include "mpiMatrix.h"

size_t poisson(size_t dimension, struct mpi_com *uplink);
size_t poisson(size_t dimension, struct mpi_com *uplink){

	struct mpiMatrix * matrix = mpiMatrix_ctor( (size_t)dimension , (size_t)dimension, *uplink );
	populate(matrix, uplink);
	double * packed = mpiMatrix_serialiseForSending(matrix, uplink);
	int * SRcounts = mpiMatrix_genCounts(matrix, uplink);
	int * SRdispl = mpiMatrix_genDispl(uplink, SRcounts);
	MPI_Alltoallv( packed, SRcounts , SRdispl, MPI_DOUBLE, matrix -> data, SRcounts, SRdispl, MPI_DOUBLE,  uplink->comm);
	matrix -> data = mpiMatrix_deserialiseAfterReception(matrix , matrix ->data, uplink);
	int * counts = mpiMatrix_genCounts(matrix, uplink);
	int * displ = mpiMatrix_genDispl(matrix, counts);
	sleep(uplink -> rank*1);
	return matrix -> width;
}
int main(int argc, char ** argv){
	struct mpi_com uplink;
	mpi_com_Init(&uplink ,&argc, & argv);
	//printf("rank: %zu nprocs: %zu\n", uplink . rank , uplink . nprocs );
	poisson(1000,  &uplink);
	mpi_com_Finalize();
	return 0;
 }
