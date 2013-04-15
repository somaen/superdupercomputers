#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "mpi.com.h"
#include "mpiMatrix.h"
#include "PrecisionTimer.h"

// fst.f
void mpiMatrix_rowfst( struct mpiMatrix * matrix );
void fst_(double *vector, int *vector_length, double *aux_mem, int *aux_mem_length); // Vector, Array Length, Auxiliary Memory, Length
void mpiMatrix_rowfst( struct mpiMatrix * matrix ){
	int aux_mem_length = 4*(int)matrix->height;
	int vector_length = (int)matrix->height;
	double * aux_mem =  calloc ( (size_t)aux_mem_length, sizeof(double));
	for ( size_t row = 0 ; row < matrix -> widthLocal ; row++){
		fst_( & matrix ->data [ row * matrix->height ] , &vector_length, aux_mem, &aux_mem_length );
	}
}
void mpiMatrix_rowifst( struct mpiMatrix * matrix );
void fstinv_(double *v, int *n, double *w, int *nn);
void mpiMatrix_rowifst( struct mpiMatrix * matrix ){
	int aux_mem_length = 4*(int)matrix->height;
	int vector_length = (int)matrix->height;
	double * aux_mem =  calloc ( (size_t)aux_mem_length, sizeof(double));
	for ( size_t row = 0 ; row < matrix -> widthLocal ; row++){
		fstinv_( & matrix ->data [ row * matrix->height ] , &vector_length, aux_mem, &aux_mem_length );
	}
}


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


double *createDiag(size_t m, size_t n) {
	double *diag = (double *) calloc(m, sizeof(double));
	for (size_t i = 0; i < m; i++) {
		diag[i] = 2.*(1. - cos((i + 1) * M_PI / (double)n));
	}
	return diag;
}

double populator(size_t x, size_t y , double scale){
	return 5*M_PI*M_PI*scale*scale*sin(M_PI*(y+1)*scale)*sin(M_PI*(x+1)*scale);
}


size_t poisson(size_t dimension, struct mpi_com *uplink, const char * resultname, const char *logfile);
size_t poisson(size_t dimension, struct mpi_com *uplink, const char * resultname, const char *logfile){
	struct mpiMatrix * matrix = mpiMatrix_ctor_habitate( (size_t)dimension , (size_t)dimension, uplink , populator);

	struct Precision_Timer divdiag;
	struct Precision_Timer fsttimer2;
	struct Precision_Timer transposetimer2;
	struct Precision_Timer ifsttimer2;
	struct Precision_Timer ifsttimer1;
	struct Precision_Timer transposetimer1;
	struct Precision_Timer fsttimer1;
	struct Precision_Timer diagTimer;
	struct Precision_Timer total;
	

	PT_start(&total);
	PT_start(&diagTimer);
	double *diag = createDiag(dimension - 1, dimension);
	PT_stop(&diagTimer);

	// Torje: Jeg kommenterte denne enda mer ut fordi jeg har populert allerede//mpiMatrix_fillValue(matrix, uplink -> rank);

	// TODO: FST
	//mpiMatrix_prettyPrint(matrix, uplink);
	PT_start(&fsttimer1);
	mpiMatrix_rowfst(matrix);
	PT_stop(&fsttimer1);

	// Torje: Jeg kommenterte denne enda mer ut fordi jeg har populert allerede//populate(matrix, uplink);
	//

	PT_start(&transposetimer1);
	mpiMatrix_transpose(matrix, uplink);
	PT_stop(&transposetimer1);

	// TODO: FSTINV
	PT_start(&ifsttimer1);
	mpiMatrix_rowifst(matrix);
	PT_stop(&ifsttimer1);

	PT_start(&divdiag);
	mpiMatrix_divByDiag(matrix, diag);
	PT_stop(&divdiag);


	// TODO FST
	PT_start(&fsttimer2);
	mpiMatrix_rowfst(matrix);
	PT_stop(&fsttimer2);

	//mpiMatrix_prettyPrint(matrix, uplink);
	PT_start(&transposetimer2);
	mpiMatrix_transpose(matrix, uplink);
	PT_stop(&transposetimer2);

	// TODO: FSTINV
	PT_start(&ifsttimer2);
	mpiMatrix_rowifst(matrix);
	PT_stop(&ifsttimer2);
	PT_stop(&total);

	// TODO: mpiMatrix_findMax(matrix);

	PT_diffTime(&total);
	PT_diffTime( & divdiag);
	PT_diffTime( & fsttimer2);
	PT_diffTime( & transposetimer2);
	PT_diffTime( & ifsttimer2);
	PT_diffTime( & ifsttimer1);
	PT_diffTime( & fsttimer1);
	PT_diffTime( & diagTimer);
   	PT_diffTime( & transposetimer1);
	//mpiMatrix_prettyPrint(matrix, uplink);
	{	
		for ( size_t process = 0; process < uplink -> nprocs ; process ++ ){
			MPI_Barrier(uplink -> comm );
			if ( process == uplink -> rank ){
				FILE *file = fopen(logfile, "a");
				fprintf(file, "process no: %zu \n", uplink->rank);
				fprintf(file, "Divided by Diagonal in :"); print_timeval(&divdiag, file); fprintf(file, "\n");
				fprintf(file, "fst 2 in: "); print_timeval(&fsttimer2, file); fprintf(file, "\n");
				fprintf(file, "tranpose 2 in:"); print_timeval(&transposetimer2, file); fprintf(file, "\n");
				fprintf(file, "inverse fst 2 in: "); print_timeval(&ifsttimer2, file); fprintf(file, "\n");
				fprintf(file, "inverse fst 1 in: "); print_timeval(&ifsttimer1, file); fprintf(file, "\n");
				fprintf(file, "transpose 1 in: "); print_timeval(&transposetimer1, file); fprintf(file, "\n");
				fprintf(file, "fst 1 in :"); print_timeval(&fsttimer1, file); fprintf(file, "\n");
				fprintf(file , "Created diagonal in: "); print_timeval(&diagTimer, file); fprintf(file, "\n");
				fprintf(file , "Total: "); print_timeval(&total, file); fprintf(file, "\n");
				fflush(file);
				fclose(file);
			}
		}
		MPI_Barrier(uplink ->comm);
	}
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
	mpi_com_Init(&uplink ,&argc, & argv);
	//printf("rank: %zu nprocs: %zu\n", uplink . rank , uplink . nprocs );
	poisson((size_t)atoi(argv[1]),  &uplink, argv[2], argv[3]);
	mpi_com_Finalize();
	return 0;
 }
