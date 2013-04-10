#pragma once
#include "mpi.com.private.h"
#include <stdio.h>
struct mpi_com {
	MPI_Comm comm;
	size_t nprocs;
	size_t rank;
};

void mpi_com_Init(struct mpi_com *uplink, int *argc, char ***argv);
void mpi_com_Finalize(void);
void generateSendCounts(struct mpi_com uplink, int *sendCounts , size_t length);
void generateSendDisplacements(struct mpi_com uplink, int *sendDisplacements, int *sendCounts);
void printCountsAndDisplacements(struct mpi_com uplink, int *sendCounts, int *sendDisplacements);
double reducePlus(double *Vector, int length) ;
void printdoubleVetor(double *Vector, int length) ;
