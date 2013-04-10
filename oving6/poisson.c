#include <stdio.h>
#include "mpi.com.h"
#include "mpiMatrix.h"

size_t poisson(size_t dimension, struct mpi_com *uplink);
size_t poisson(size_t dimension, struct mpi_com *uplink){

	struct mpiMatrix * matrix = mpiMatrix_ctor( (size_t)dimension , (size_t)dimension, *uplink );
	populate(matrix, uplink);
	for ( size_t i = 0 ; i < dimension ; i++){
		printf("%lf ",matrix -> data[ i]); 
	}
	printf("\n "); 
	double * packed = mpiMatrix_serialiseForSending(matrix, uplink);

	int * SRcounts = calloc( uplink -> nprocs , sizeof(int));
	generateSendCounts();
	int * SRdispl = calloc( uplink -> nprocs , sizeof(int));
	generateSendDisplacements();
	mpiMatrix_deserialiseAfterReception(matrix, packed, uplink);

	return matrix -> width;
}
int main(int argc, char ** argv){
	struct mpi_com uplink;
	mpi_com_Init(&uplink ,&argc, & argv);
	printf("%zu\n", poisson(10,  &uplink));
	mpi_com_Finalize();
	return 0;
 }
