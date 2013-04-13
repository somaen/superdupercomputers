#include <stdlib.h>
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
void mpiMatrix_dtor(struct mpiMatrix *matrix);
double *getVector(struct mpiMatrix, size_t column);
void populate( struct mpiMatrix * matrix , struct mpi_com *uplink);
int * mpiMatrix_genCounts( struct mpiMatrix * matrix , struct mpi_com *uplink);
double * mpiMatrix_serialiseForSending( struct mpiMatrix * matrix , struct mpi_com *uplink);
double *mpiMatrix_deserialiseAfterReception( struct mpiMatrix * matrix, double * data, struct mpi_com *uplink);
int * mpiMatrix_genCounts( struct mpiMatrix * matrix , struct mpi_com *uplink);
int * mpiMatrix_genDispl(struct mpi_com *uplink, int * counts);
void mpiMatrix_transpose( struct mpiMatrix * matrix, struct mpi_com *uplink);
