#include <mpi.h>
#include <string.h>
#include "mpiMatrix.h"


void mpiMatrix_print( struct mpiMatrix * matrix , struct mpi_com *uplink) {
	for ( size_t column = 0 ; column < matrix -> widthLocal ; column ++ ){
		for ( size_t row = 0 ; row < matrix -> height ; row++){
			printf("%5.0lf ", matrix -> data[column*matrix->height + row ]);
		}
		printf("\n");
	}
}

void populate( struct mpiMatrix * matrix , struct mpi_com *uplink) {
	int * counts =  mpiMatrix_genCounts(matrix, uplink);
	int * displ =  mpiMatrix_genDispl(uplink, counts);
	//printf("rank: %zu nprocs: %zu\n", uplink -> rank , uplink -> nprocs );
	for ( size_t i = 0 ; i < matrix -> widthLocal; i++ ){
		for ( size_t j = 0 ; j < matrix->height ; j++ ){
			matrix -> data[
				matrix->height*i 
				+j] 
				= 
				uplink->rank * 1000
				+ j 
				+ i*matrix->height;
		}
	}
}

#ifdef standalone
int main(int argc, char** argv){
	struct mpi_com uplink;
	mpi_com_Init(&uplink, &argc, &argv);
	struct mpiMatrix * matrix = mpiMatrix_ctor(100,100, uplink); 
	printf("%p\n", matrix );
	mpi_com_Finalize();
	return 0;
}
#endif

int * mpiMatrix_genCounts( struct mpiMatrix * matrix , struct mpi_com *uplink) {
	size_t mynumber = matrix -> height /  uplink -> nprocs;
	if ( uplink->rank<( matrix -> height % uplink -> nprocs)){
		mynumber +=1;
	}
	int *counts = calloc( uplink->nprocs, sizeof(int));
	for ( size_t process = 0 ; 
			process < matrix -> height % uplink -> nprocs; 
			process++) {
		counts[process] = (int)(( matrix -> height / uplink -> nprocs + 1 ) *mynumber);

	}
	for ( size_t process = matrix -> height % uplink -> nprocs; 
			process < uplink -> nprocs; 
			process++) {
		counts[process] = (int)((matrix -> height / uplink -> nprocs) *mynumber);
	}
	return counts;
}

int * mpiMatrix_genDispl(struct mpi_com *uplink, int * counts) {
	int *displ = calloc( uplink->nprocs, sizeof(int));
	displ[0] = 0;
	for ( size_t process = 1 ; 
			process < uplink -> nprocs; 
			process++) {
		displ[process] = displ[process-1]+ counts[process-1];
	}
	return displ;
}

double * mpiMatrix_serialiseForSending( struct mpiMatrix * matrix , struct mpi_com *uplink) {
	size_t size = matrix -> widthLocal*matrix->height;
	double *serialisedArray = calloc( size, sizeof(double));
	size_t serialIndex = 0;
	for (size_t process = 0; 
			process < ( matrix -> height % uplink->nprocs ); 
			process++) {
		size_t startindex = 0;
		size_t vectorLength = (matrix -> height / uplink->nprocs + 1);
		for( size_t column = 0 ; column < matrix -> widthLocal ; column ++ ){
			size_t matrixIndex = column * matrix -> height + process * vectorLength + startindex;
			memcpy ( & serialisedArray[serialIndex] , & matrix->data[matrixIndex], vectorLength *sizeof(double));
			serialIndex += vectorLength;
		}
	}
	for (size_t process = (matrix -> height % uplink -> nprocs); 
			process < uplink->nprocs; 
			process++) {
		size_t startindex = matrix -> height % uplink -> nprocs ;
		size_t vectorLength = (matrix -> height / uplink->nprocs);
		for( size_t column = 0 ; column < matrix -> widthLocal ; column ++ ){
			size_t matrixIndex = column * matrix -> height + process * vectorLength + startindex;
			memcpy ( & serialisedArray[serialIndex] , & matrix->data[matrixIndex], vectorLength *sizeof(double));
			serialIndex += vectorLength;
		}
	}
	return serialisedArray;
}

double *mpiMatrix_deserialiseAfterReception( struct mpiMatrix * matrix, double * data, struct mpi_com *uplink ){
	double * cVectors = calloc ( matrix -> height * matrix-> widthLocal , sizeof(double)); 
	int * sendcounts = mpiMatrix_genCounts( matrix , uplink);
	int * displacements = mpiMatrix_genDispl( uplink, sendcounts);
	for ( size_t index = 0; 
			index <	matrix -> height *matrix -> widthLocal ; 
			index ++ ){
		size_t newcoord =  (index / matrix -> widthLocal ) + (index % matrix->widthLocal)*matrix->height;
		//printf("%zu : %zu\n", index , newcoord);
		cVectors[newcoord] = data [index];
	}
	return cVectors;
}

struct mpiMatrix *mpiMatrix_ctor(size_t height, size_t width, struct mpi_com uplink){
	struct mpiMatrix *matrix = calloc(1, sizeof(struct mpiMatrix));
	size_t widthLocal = width / uplink.nprocs + ((uplink.rank < (width%uplink.nprocs)) ? 1:0);
	size_t offset;
	/*

	if ( uplink . rank >= ( width % uplink.nprocs )){
		offset = (width%uplink.nprocs) * (width / uplink.nprocs+1);
		offset += (uplink.nprocs- (width%uplink.nprocs))*(width / uplink.nprocs+1);
	}
	else{
		offset = (width%uplink.nprocs) * (width / uplink.nprocs+1);
	}*/

	matrix->data= calloc(height*width, sizeof(double));
	//matrix->widthOffset = offset;
	matrix->width = widthLocal;
	matrix->widthLocal = widthLocal;
	matrix->height = height;
	matrix->uplink = uplink;
	populate ( matrix , &uplink);
	return matrix;
}

void mpiMatrix_dtor(struct mpiMatrix *matrix) {
	free(matrix->data);
	free(matrix);
}

void mpiMatrix_transpose( struct mpiMatrix * matrix, struct mpi_com *uplink) {
	double * packed = mpiMatrix_serialiseForSending(matrix, uplink);
	int * SRcounts = mpiMatrix_genCounts(matrix, uplink);
	int * SRdispl = mpiMatrix_genDispl(uplink, SRcounts);

	MPI_Alltoallv( packed, SRcounts , SRdispl, MPI_DOUBLE, matrix->data, SRcounts, SRdispl, MPI_DOUBLE,  uplink->comm);
	matrix -> data = mpiMatrix_deserialiseAfterReception(matrix , matrix->data, uplink);

	free(SRcounts);
	free(SRdispl);
}

