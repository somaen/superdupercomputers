#include <stdio.h>
#include "mpi.com.h"
#include "mpiMatrix.h"


size_t poisson(size_t dimension, struct mpi_com *uplink);
size_t poisson(size_t dimension, struct mpi_com *uplink){

	struct mpiMatrix * matrix = mpiMatrix_ctor( (size_t)dimension , (size_t)dimension, *uplink );
	populate(matrix, uplink);
	mpiMatrix_print(matrix, uplink);

	mpiMatrix_transpose(matrix, uplink);

	mpiMatrix_print(matrix, uplink);

	int * counts = mpiMatrix_genCounts(matrix, uplink);
	int * displ = mpiMatrix_genDispl(matrix, counts);
	sleep(uplink -> rank*1);

	size_t result = matrix->width;

	mpiMatrix_dtor(matrix);
	free(counts);
	free(displ);

	return result;
}
int main(int argc, char ** argv){
	struct mpi_com uplink;
	mpi_com_Init(&uplink ,&argc, & argv);
	//printf("rank: %zu nprocs: %zu\n", uplink . rank , uplink . nprocs );
	poisson(atoi(argv[1]),  &uplink);
	mpi_com_Finalize();
	return 0;
 }
