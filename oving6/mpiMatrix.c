#include <mpi.h>
#include <string.h>
#include "mpiMatrix.h"


struct mpiMatrix *mpiMatrix_ctor_habitate(size_t height, size_t width, struct mpi_com *uplink, double(*habitant)(size_t, size_t, double)) {
	struct mpiMatrix *matrix = mpiMatrix_ctor(height, width, *uplink);
	int *counts = mpiMatrix_genCounts(matrix, uplink);
	int *displ = mpiMatrix_genDispl(uplink, counts);
	double scale  = 1. / (matrix -> height + 1) ;
	for (size_t y = 0 ; y < matrix -> widthLocal ; y++) {
		for (size_t x = 0 ; x < matrix -> height ; x++) {
			matrix -> data[y * matrix -> height + x ] = habitant(x, y + (size_t)displ[uplink ->rank] / matrix -> width, scale);
		}
	}
	free(counts);
	free(displ);
	return matrix;
}
void mpiMatrix_minus( struct mpiMatrix * matrix, struct mpi_com *uplink ,double(*habitant)(size_t, size_t, double)){
	int *counts = mpiMatrix_genCounts(matrix, uplink);
	int *displ = mpiMatrix_genDispl(uplink, counts);
	double scale  = 1. / (matrix -> height + 1) ;
	for (size_t y = 0 ; y < matrix -> widthLocal ; y++) {
		for (size_t x = 0 ; x < matrix -> height ; x++) {
			matrix -> data[y * matrix -> height + x ] -= habitant(x, y + (size_t)displ[uplink ->rank] / matrix -> width, scale);
		}
	}
	free(counts);
	free(displ);
}

// fst.f
void fst_(double *vector, int *vector_length, double *aux_mem, int *aux_mem_length); 
void fstinv_(double *v, int *n, double *w, int *nn);

void mpiMatrix_rowfst(struct mpiMatrix *matrix) {
	int aux_mem_length = 4 * ((int)matrix->height + 1);
	int vector_length = (int)matrix->height + 1;
	#pragma omp parallel
	{
		double *aux_mem =  calloc((size_t)aux_mem_length, sizeof(double));
		#pragma omp parallel for
		for (int row = 0 ; row < matrix -> widthLocal ; row++) {
			fst_(& matrix ->data [ row * matrix->height ] , &vector_length, aux_mem, &aux_mem_length);
		}
		free(aux_mem);
	}
}

void mpiMatrix_rowifst(struct mpiMatrix *matrix) {
	int aux_mem_length = 4 * ((int)matrix->height + 1);
	int vector_length = (int)matrix->height + 1;
	#pragma omp parallel
	{
		double *aux_mem =  calloc((size_t)aux_mem_length, sizeof(double));
		#pragma omp parallel for
		for (int row = 0 ; row < matrix -> widthLocal ; row++) {
			fstinv_(& matrix ->data [ row * matrix->height ] , &vector_length, aux_mem, &aux_mem_length);
		}
		free(aux_mem);
	}
}



void mpiMatrix_divByDiag(struct mpiMatrix *matrix, double *diag) {
	for (size_t i = 0; i < matrix -> widthLocal; i++) {
		for (size_t j = 0; j < matrix->height; j++) {
			matrix->data[matrix->height * i + j] = matrix->data[matrix->height * i + j] / (diag[i] + diag[j]);
		}
	}
}


double mpiMatrix_findMax(struct mpiMatrix *matrix) {
	double umax = 0.0;
	for (size_t i = 0; i < matrix -> widthLocal; i++) {
		for (size_t j = 0; j < matrix->height; j++) {
			if (matrix->data[matrix->height * i + j] > umax) {
				umax = matrix->data[matrix->height * i + j];
			}
		}
	}
	return umax;
}

void mpiMatrix_fillValue(struct mpiMatrix *matrix, double value) {
	for (size_t i = 0; i < matrix -> widthLocal; i++) {
		for (size_t j = 0; j < matrix->height; j++) {
			matrix->data[
			    matrix->height * i
			    + j]
			    =
			        value;
		}
	}
}
void mpiMatrix_fprint(struct mpiMatrix *matrix, const char *filename) {
	FILE *file = fopen(filename, "a");
	for (size_t column = 0 ; column < matrix -> widthLocal ; column ++) {
		for (size_t row = 0 ; row < matrix -> height ; row++) {
			fprintf(file, "%.16lf ", matrix -> data[column * matrix->height + row ]);
		}
		fprintf(file, "\n");
	}
	fflush(file);
	fclose(file);
}

void mpiMatrix_fprettyPrint(struct mpiMatrix *matrix , struct mpi_com *uplink, const char *filename) {
	if (uplink -> rank == 0) {
		FILE *file = fopen(filename, "w");
		fflush(file);
		fclose(file);
	}
	for (size_t process = 0; process < uplink -> nprocs ; process ++) {
		MPI_Barrier(uplink -> comm);
		if (process == uplink -> rank) {
			mpiMatrix_fprint(matrix, filename);
		}
	}
	MPI_Barrier(uplink ->comm);
}


void mpiMatrix_print(struct mpiMatrix *matrix) {
	for (size_t column = 0 ; column < matrix -> widthLocal ; column ++) {
		for (size_t row = 0 ; row < matrix -> height ; row++) {
			printf("%e ", matrix -> data[column * matrix->height + row ]);
		}
		printf("\n");
	}
}

void mpiMatrix_prettyPrint(struct mpiMatrix *matrix , struct mpi_com *uplink) {
	for (size_t process = 0; process < uplink -> nprocs ; process ++) {
		MPI_Barrier(uplink -> comm);
		if (process == uplink -> rank) {
			mpiMatrix_print(matrix);
		}
	}
	MPI_Barrier(uplink ->comm);
}

void populate(struct mpiMatrix *matrix , struct mpi_com *uplink) {
	int *counts =  mpiMatrix_genCounts(matrix, uplink);
	int *displ =  mpiMatrix_genDispl(uplink, counts);
	for (size_t i = 0 ; i < matrix -> widthLocal; i++) {
		for (size_t j = 0 ; j < matrix->height ; j++) {
			matrix -> data[
			    matrix->height * i
			    + j]
			    =
			        uplink->rank * 1000
			        + j
			        + i * matrix->height;
		}
	}
	free(counts);
	free(displ);
}

#ifdef standalone
int main(int argc, char **argv) {
	struct mpi_com uplink;
	mpi_com_Init(&uplink, &argc, &argv);
	struct mpiMatrix *matrix = mpiMatrix_ctor(100, 100, uplink);
	printf("%p\n", matrix);
	mpi_com_Finalize();
	mpiMatrix_dtor(matrix);
	return 0;
}
#endif

int *mpiMatrix_genCounts(struct mpiMatrix *matrix , struct mpi_com *uplink) {
	size_t mynumber = matrix -> height /  uplink -> nprocs;
	if (uplink->rank < (matrix -> height % uplink -> nprocs)) {
		mynumber += 1;
	}
	int *counts = calloc(uplink->nprocs, sizeof(int));
	for (size_t process = 0 ;
	        process < matrix -> height % uplink -> nprocs;
	        process++) {
		counts[process] = (int)((matrix -> height / uplink -> nprocs + 1) * mynumber);

	}
	for (size_t process = matrix -> height % uplink -> nprocs;
	        process < uplink -> nprocs;
	        process++) {
		counts[process] = (int)((matrix -> height / uplink -> nprocs) * mynumber);
	}
	return counts;
}

int *mpiMatrix_genDispl(struct mpi_com *uplink, int *counts) {
	int *displ = calloc(uplink->nprocs, sizeof(int));
	displ[0] = 0;
	for (size_t process = 1 ;
	        process < uplink -> nprocs;
	        process++) {
		displ[process] = displ[process - 1] + counts[process - 1];
	}
	return displ;
}

void mpiMatrix_swapDataAux(struct mpiMatrix *matrix) {
	double *temp = matrix ->data;
	matrix->data = matrix->aux;
	matrix->aux = temp;
}

void mpiMatrix_serialiseForSending(struct mpiMatrix *matrix , struct mpi_com *uplink) {
	size_t serialIndex = 0;
	for (size_t process = 0;
	        process < (matrix -> height % uplink->nprocs);
	        process++) {
		size_t startindex = 0;
		size_t vectorLength = (matrix -> height / uplink->nprocs + 1);
		for (size_t column = 0 ; column < matrix -> widthLocal ; column ++) {
			size_t matrixIndex = column * matrix -> height + process * vectorLength + startindex;
			memcpy(& matrix->aux[serialIndex] , & matrix->data[matrixIndex], vectorLength * sizeof(double));
			serialIndex += vectorLength;
		}
	}
	for (size_t process = (matrix -> height % uplink -> nprocs);
	        process < uplink->nprocs;
	        process++) {
		size_t startindex = matrix -> height % uplink -> nprocs ;
		size_t vectorLength = (matrix -> height / uplink->nprocs);
		for (size_t column = 0 ; column < matrix -> widthLocal ; column ++) {
			size_t matrixIndex = column * matrix -> height + process * vectorLength + startindex;
			memcpy(& matrix->aux[serialIndex] , & matrix->data[matrixIndex], vectorLength * sizeof(double));
			serialIndex += vectorLength;
		}
	}
	mpiMatrix_swapDataAux(matrix);
}

void mpiMatrix_deserialiseAfterReception(struct mpiMatrix *matrix) {
	for (size_t index = 0;
	        index < matrix -> height * matrix -> widthLocal ;
	        index ++) {
		size_t newcoord = (index / matrix -> widthLocal) + (index % matrix->widthLocal) * matrix->height;
		matrix->aux[newcoord] = matrix->data [index];
	}
	mpiMatrix_swapDataAux(matrix);
}

struct mpiMatrix *mpiMatrix_ctor(size_t height, size_t width, struct mpi_com uplink) {
	struct mpiMatrix *matrix = calloc(1, sizeof(struct mpiMatrix));
	size_t widthLocal = width / uplink.nprocs + ((uplink.rank < (width % uplink.nprocs)) ? 1 : 0);

	matrix->data = calloc(height *width, sizeof(double));
	matrix->aux = calloc(height *width, sizeof(double));
	
	matrix->width = widthLocal;
	matrix->widthLocal = widthLocal;
	matrix->height = height;
	matrix->uplink = uplink;
	populate(matrix , &uplink);
	return matrix;
}

void mpiMatrix_dtor(struct mpiMatrix *matrix) {
	free(matrix->data);
	free(matrix->aux);
	free(matrix);
}

void mpiMatrix_transpose(struct mpiMatrix *matrix, struct mpi_com *uplink) {
	mpiMatrix_serialiseForSending(matrix, uplink);
	int *SRcounts = mpiMatrix_genCounts(matrix, uplink);
	int *SRdispl = mpiMatrix_genDispl(uplink, SRcounts);

	MPI_Alltoallv(matrix->data, SRcounts , SRdispl, MPI_DOUBLE, matrix->aux, SRcounts, SRdispl, MPI_DOUBLE,  uplink->comm);
	mpiMatrix_swapDataAux(matrix);
	mpiMatrix_deserialiseAfterReception(matrix);

	free(SRcounts);
	free(SRdispl);
}

