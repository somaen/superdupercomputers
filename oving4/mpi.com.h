#pragma once
#include "mpi.com.private.h"
#include <stdio.h>
struct mpi_com {
	size_t nprocs;
	size_t rank;
} uplink;

void mpi_com_Init(int *argc, char ***argv);
void mpi_com_Finalize(void);
void generateSendCounts(int *sendCounts , size_t length);
void generateSendDisplacements(int *sendDisplacements, int *sendCounts);
void printCountsAndDisplacements(int *sendCounts, int *sendDisplacements);
double reducePlus(double *Vector, int length) ;
void printdoubleVetor(double *Vector, int length) ;
