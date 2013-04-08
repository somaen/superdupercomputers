#include <stdlib.h>
#include <mpi.h>
#include <string.h>
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
	mpi_com_Init(&uplink, &argc, &argv);
	struct mpiMatrix * matrix = mpiMatrix_ctor(100,100, uplink); 
	printf("%p\n", matrix );
	mpi_com_Finalize();
	return 0;
}

int * mpiMatrix_genCounts( struct mpiMatrix * matrix , struct mpi_com *uplink) {
	int mynumber = matrix -> height /  uplink -> nprocs;
	if ( uplink->rank<( matrix -> height % uplink -> nprocs)){
		mynumber +=1;
	}
	int *counts = calloc( uplink->nprocs, sizeof(int));
	for ( size_t process = 0 ; 
			process < matrix -> height % uplink -> nprocs; 
			process++) {
		counts[process] = ( matrix -> height / uplink -> nprocs + 1 ) *mynumber;

	}
	for ( size_t process = process < matrix -> height % uplink -> nprocs; 
			process < uplink -> nprocs; 
			process++) {
		counts[process] = (matrix -> height / uplink -> nprocs) *mynumber;;
	}
	return counts;
}

int * mpiMatrix_genDispl( struct mpiMatrix * matrix , struct mpi_com *uplink, int * counts) {
	int *displ = calloc( uplink->nprocs, sizeof(int));
	displ[0] = 0;
	for ( size_t process = 1 ; 
			process < uplink -> nprocs; 
			process++) {
		displ[process] += counts[process-1];
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
			size_t index = column * matrix -> height + process * vectorLength + startindex;
			size_t matrixIndex = column * matrix -> height + process * vectorLength + startindex;
			memcpy ( & serialisedArray[serialIndex] , & matrix->data[matrixIndex], vectorLength *sizeof(double));
			serialIndex += vectorLength;
		}
	}
	return serialisedArray;
}

double *mpiMatrix_deserialiseAfterReception( struct mpiMatrix * matrix, double * data, struct mpi_com *uplink){
	double * cVectors = calloc ( matrix -> height * matrix-> widthLocal , sizeof(double)); 
	for ( size_t column = 0 ;  column <matrix-> widthLocal ; column++){
		size_t offset = 0;
		for ( size_t process = 0 ; 
				process < matrix -> height % uplink -> nprocs; 
				process++) {
			size_t localHeight = matrix -> height / uplink -> nprocs + 1; 
			for ( size_t localRow = 0 ; localRow < localHeight ;
					localRow++ ){
				cVectors[offset
					+ localRow
					+ column*matrix -> height ] 
					= 
					data[ offset
					+localRow
					+column*localHeight ];
			}
			offset += localHeight;
		}
		for ( size_t process = process < matrix -> height % uplink -> nprocs; 
				process < uplink -> nprocs; 
				process++) {
			size_t localHeight = matrix -> height / uplink -> nprocs + 1; 
			for ( size_t localRow = 0 ; localRow < localHeight ;
					localRow++ ){
				cVectors[offset
					+ localRow
					+ column*matrix -> height ] 
					= 
					data[ offset
					+localRow
					+column*localHeight ];
			}
			offset += localHeight;
		}
	}
	return cVectors;
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
