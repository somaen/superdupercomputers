#include <stdlib.h>
#include "mpi.com.h"

struct mpiMatrix{
	size_t height;
	size_t width;
	size_t widthOffset;
	size_t widthLocal;
	struct mpi_com uplink;
	double *data;
	double *aux;
};

void mpiMatrix_fprettyPrint( struct mpiMatrix * matrix , struct mpi_com *uplink, const char * filename);
void mpiMatrix_fprint( struct mpiMatrix * matrix, const char * filename);
void mpiMatrix_prettyPrint( struct mpiMatrix * matrix , struct mpi_com *uplink);
struct mpiMatrix *mpiMatrix_ctor_habitate(size_t height, size_t width, struct mpi_com *uplink, double (*habitant)(size_t, size_t, double));
struct mpiMatrix *mpiMatrix_ctor(size_t height, size_t width, struct mpi_com uplink);
void mpiMatrix_dtor(struct mpiMatrix *matrix);
double *getVector(struct mpiMatrix, size_t column);
void populate( struct mpiMatrix * matrix , struct mpi_com *uplink);
int * mpiMatrix_genCounts( struct mpiMatrix * matrix , struct mpi_com *uplink);
double * mpiMatrix_serialiseForSending( struct mpiMatrix * matrix , struct mpi_com *uplink);
double *mpiMatrix_deserialiseAfterReception( struct mpiMatrix * matrix, double * data);
int * mpiMatrix_genCounts( struct mpiMatrix * matrix , struct mpi_com *uplink);
int * mpiMatrix_genDispl(struct mpi_com *uplink, int * counts);
void mpiMatrix_transpose( struct mpiMatrix * matrix, struct mpi_com *uplink);
void mpiMatrix_print( struct mpiMatrix * matrix );


double mpiMatrix_findMax(struct mpiMatrix *matrix);
void mpiMatrix_divByDiag(struct mpiMatrix *matrix, double *diag);
void mpiMatrix_fillValue(struct mpiMatrix *matrix, double value);
