#include <stdio.h>
#include <math.h>
#include <signal.h>
#include "mpi.com.h"
#include "mpiMatrix.h"
#include <unistd.h>

// fst.f
void mpiMatrix_rowfst( struct mpiMatrix * matrix );
void fst_(double *vector, int *vector_length, double *aux_mem, int *aux_mem_length); // Vector, Array Length, Auxiliary Memory, Length
void mpiMatrix_rowfst( struct mpiMatrix * matrix ){
	int aux_mem_length = 4*matrix->height;
	int vector_length = matrix->height;
	double * aux_mem =  calloc ( aux_mem_length, sizeof(double));
	for ( size_t row = 0 ; row < matrix -> widthLocal ; row++){
		fst_( & matrix ->data [ row * matrix->height ] , &vector_length, aux_mem, &aux_mem_length );
	}
}
void fstinv_(double *v, int *n, double *w, int *nn);
void mpiMatrix_rowifst( struct mpiMatrix * matrix ){
	int aux_mem_length = 4*matrix->height;
	int vector_length = matrix->height;
	double * aux_mem =  calloc ( aux_mem_length, sizeof(double));
	for ( size_t row = 0 ; row < matrix -> widthLocal ; row++){
		fstinv_( & matrix ->data [ row * matrix->height ] , &vector_length, aux_mem, &aux_mem_length );
	}
}


static void catch_function(int signal);
double populator(size_t x, size_t y , double scale);
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

double populator(size_t x, size_t y , double scale){
	return 5*M_PI*M_PI*sin(M_PI*y*scale)*sin(M_PI*x*scale);
}


static void catch_function(int signal) {
	mpi_com_Finalize();
	exit(0);
}

size_t poisson(size_t dimension, struct mpi_com *uplink, const char * resultname, const char *logfile);
size_t poisson(size_t dimension, struct mpi_com *uplink, const char * resultname, const char *logfile){
	struct mpiMatrix * matrix = mpiMatrix_ctor_habitate( (size_t)dimension , (size_t)dimension, uplink , populator);
	

	double *diag = createDiag(dimension - 1, dimension);
	double h = 1. / (double) dimension;

	// Torje: Jeg kommenterte denne enda mer ut fordi jeg har populert allerede//mpiMatrix_fillValue(matrix, uplink -> rank);

	// TODO: FST
	//mpiMatrix_prettyPrint(matrix, uplink);
	mpiMatrix_rowfst(matrix);

	// Torje: Jeg kommenterte denne enda mer ut fordi jeg har populert allerede//populate(matrix, uplink);
	//

	mpiMatrix_transpose(matrix, uplink);

	// TODO: FSTINV
	mpiMatrix_rowifst(matrix);

	mpiMatrix_divByDiag(matrix, diag);


	// TODO FST
	mpiMatrix_rowfst(matrix);

	//mpiMatrix_prettyPrint(matrix, uplink);
	mpiMatrix_transpose(matrix, uplink);

	// TODO: FSTINV
	mpiMatrix_rowifst(matrix);

	// TODO: mpiMatrix_findMax(matrix);

	//mpiMatrix_prettyPrint(matrix, uplink);
	
	mpiMatrix_fprettyPrint(matrix, uplink, resultname);

	int * counts = mpiMatrix_genCounts(matrix, uplink);
	int * displ = mpiMatrix_genDispl(uplink, counts);
	sleep((unsigned int)uplink -> rank*1);

	size_t result = matrix->width;

	mpiMatrix_dtor(matrix);
	free(counts);
	free(displ);

	free(diag);

	return result;
}
int main(int argc, char ** argv){
	struct mpi_com uplink;
//	signal(SIGSEGV, catch_function);
//	signal(SIGABRT, catch_function);
	mpi_com_Init(&uplink ,&argc, & argv);
	//printf("rank: %zu nprocs: %zu\n", uplink . rank , uplink . nprocs );
	poisson((size_t)atoi(argv[1]),  &uplink, argv[2], argv[3]);
	mpi_com_Finalize();
	return 0;
 }
