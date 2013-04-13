#include <stdio.h>
#include <math.h>
#include "mpi.com.h"
#include "mpiMatrix.h"

// fst.f
void fst_(double *v, int *n, double *w, int *nn); // Vector, Array Length, Auxiliary Memory, Length
void fstinv_(double *v, int *n, double *w, int *nn);

void mpiMatrix_fillValue(struct mpiMatrix *matrix, double value);
double mpiMatrix_findMax(struct mpiMatrix *matrix);
void mpiMatrix_divByDiag(struct mpiMatrix *matrix, double *diag);
double *createDiag(size_t m, size_t n);
/*
void FST_wrap_m(double **v, int n, double *w, int nn, int m) {
	for (size_t i = 0; i < m; i++) {
		fst_(v[i], &n, w, &nn);
	}
}

void FSTINV_wrap_m(double **v, int n, double *w, int nn, int m) {
	for (size_t i = 0; i < m; i++) {
		fstinv_(v, &n, w, &nn);
	}
}*/

void mpiMatrix_fillValue(struct mpiMatrix *matrix, double value) {
	for (size_t i = 0; i < matrix -> widthLocal; i++) {
		for (size_t j = 0; j < matrix->height; j++) {
			matrix->data[
				matrix->height*i
				+j]
				=
				value;
		}
	}
}

double mpiMatrix_findMax(struct mpiMatrix *matrix) {
	double umax = 0.0;
	for (size_t i = 0; i < matrix -> widthLocal; i++) {
		for (size_t j = 0; j < matrix->height; j++) {
			if (matrix->data[matrix->height*i + j] > umax) {
				umax = matrix->data[matrix->height*i + j];
			}
		}
	}
	return umax;
}

void mpiMatrix_divByDiag(struct mpiMatrix *matrix, double *diag) {
	for (size_t i = 0; i < matrix -> widthLocal; i++) {
		for (size_t j = 0; j < matrix->height; j++) {
			matrix->data[matrix->height*i + j] = matrix->data[matrix->height*i + j] / (diag[i] + diag[j]);
		}
	}

}

double *createDiag(size_t m, size_t n) {
	double *diag = (double *) calloc(m, sizeof(double));
	for (size_t i = 0; i < m; i++) {
		diag[i] = 2.*(1. - cos((i + 1) * M_PI / (double)n));
	}
	return diag;
}

size_t poisson(size_t dimension, struct mpi_com *uplink);
size_t poisson(size_t dimension, struct mpi_com *uplink){
	struct mpiMatrix * matrix = mpiMatrix_ctor( (size_t)dimension , (size_t)dimension, *uplink );

	double *diag = createDiag(dimension - 1, dimension);
	double h = 1. / (double) dimension;

	mpiMatrix_fillValue(matrix, h * h);

	// TODO: FST

	populate(matrix, uplink);
	mpiMatrix_print(matrix, uplink);

	mpiMatrix_transpose(matrix, uplink);

	// TODO: FSTINV

	// mpiMatrix_divByDiag(matrix, diag);


	// TODO FST

	// mpiMatrix_transpose(matrix, uplink);

	// TODO: FSTINV

	// TODO: mpiMatrix_findMax(matrix);

	mpiMatrix_print(matrix, uplink);

	int * counts = mpiMatrix_genCounts(matrix, uplink);
	int * displ = mpiMatrix_genDispl(matrix, counts);
	sleep(uplink -> rank*1);

	size_t result = matrix->width;

	mpiMatrix_dtor(matrix);
	free(counts);
	free(displ);

	free(diag);

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
