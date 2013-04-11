#include <stdio.h>
#include "mpi.com.h"
#include "mpiMatrix.h"

size_t poisson(size_t dimension, struct mpi_com *uplink);
size_t poisson(size_t dimension, struct mpi_com *uplink){

	struct mpiMatrix * matrix = mpiMatrix_ctor( (size_t)dimension , (size_t)dimension, *uplink );
	populate(matrix, uplink);
	printf("\n "); 
	double * packed = mpiMatrix_serialiseForSending(matrix, uplink);
	/*printf("%zu: \n", uplink -> rank );
	for ( size_t j = 0 ; j < matrix -> widthLocal ; j++ ){
		for ( size_t i = 0 ; i < matrix -> height ; i++){	
			printf("%.0lf ",matrix -> data[ i ]); 
		}
		printf("\n");
	}*/

	int * SRcounts = mpiMatrix_genCounts(matrix, uplink);
	int * SRdispl = mpiMatrix_genDispl(uplink, SRcounts);
	for ( int i  = 0 ; i < uplink -> nprocs ; i++){
		printf("%d:%d ," ,SRcounts[i], SRdispl[i]);
	}
	printf("\n");
	MPI_Alltoallv( packed, SRcounts , SRdispl, MPI_DOUBLE, matrix -> data, SRcounts, SRdispl, MPI_DOUBLE,  uplink->comm);

	
	printf("%zu: ", uplink -> rank );
	for ( size_t j = 0 ; j < matrix -> widthLocal ; j++ ){
		for ( size_t i = 0 ; i < matrix -> height ; i++){	
			printf("%.0lf ",matrix -> data[ i ]); 
		}
		printf("\n");
	}
	mpiMatrix_deserialiseAfterReception(matrix, packed, uplink);
	return matrix -> width;
}
int main(int argc, char ** argv){
	struct mpi_com uplink;
	mpi_com_Init(&uplink ,&argc, & argv);
	poisson(10,  &uplink);
	mpi_com_Finalize();
	return 0;
 }
