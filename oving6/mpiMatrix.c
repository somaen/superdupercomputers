#include <stdlib.h>
#include <mpi.h>
#include "mpi.com.h"


struct mpiMatrix{
	size_t height;
	size_t width;
	size_t widthOffset;
	size_t widthLocal;
	struct mpi_com uplink;
	double *data;
};

struct mpiMatrix *mpiMatrix_ctor(size_t height, size_t width, struct mpi_com uplink);
double *getVector(struct mpiMatrix, size_t column);
void populate( struct mpiMatrix * matrix , struct mpi_com *uplink) {
	for ( int i = 0 ; i < matrix -> widthLocal*matrix->height ; i++ ){
		matrix -> data[i] = uplink->rank;
	}
}

int main(int argc, char** argv){
	struct mpi_com uplink;
	//uplink.nprocs = 1;
	//uplink.rank =0;
	mpi_com_Init(&uplink, &argc, &argv);
	struct mpiMatrix * matrix = mpiMatrix_ctor(100,100, uplink); 
	printf("%p\n", matrix );
	mpi_com_Finalize();
	return 0;
}

struct mpiMatrix *mpiMatrix_ctor(size_t height, size_t width, struct mpi_com uplink){
	struct mpiMatrix *matrix = calloc(1, sizeof(struct mpiMatrix));
	size_t widthLocal = width / uplink.nprocs + ((uplink.rank > (width%uplink.nprocs)) ? 1:0);
	size_t offset;

	if ( uplink.rank>= ( width % uplink.nprocs )){
		offset = (width%uplink.nprocs) * (width / uplink.nprocs+1);
		offset += (uplink.nprocs- (width%uplink.nprocs))*(width / uplink.nprocs+1);
	}
	else{
		offset = (width%uplink.nprocs) * (width / uplink.nprocs+1);
	}

	matrix->data= calloc(height*width, sizeof(double));
	matrix->widthOffset = offset;
	matrix->width = widthLocal;
	matrix->height = height;
	matrix->uplink = uplink;
	populate ( matrix , &uplink);
	return matrix;
}
