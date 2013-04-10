#include "mpi.com.h"

void mpi_com_Init(struct mpi_com *uplink, int *argc, char ***argv) {
	MPI_Init(argc, argv);
	int irank, inprocs;
	MPI_Comm_rank(MPI_COMM_WORLD, &irank);
	
	MPI_Comm_size(MPI_COMM_WORLD, &inprocs);
	uplink->rank = (size_t)irank;
	uplink->nprocs = (size_t)inprocs;
	fprintf( stderr,"rank: %zd nprocs: %zd\n", uplink->rank, uplink->nprocs);
}

void mpi_com_Finalize(void) {
	MPI_Finalize();
}

void generateSendCounts(struct mpi_com uplink, int *sendCounts , size_t length) {
	for (size_t i = 0; i < (length % uplink.nprocs) ; i ++) {
		sendCounts[i] = (int)(length / uplink.nprocs + 1);
	}
	for (size_t i = (length % uplink.nprocs); i < uplink.nprocs ; i++) {
		sendCounts[i] = (int)(length / uplink.nprocs);
	}
}

void generateSendDisplacements(struct mpi_com uplink, int *sendDisplacements, int *sendCounts) {
	for (size_t i = 1; i <  uplink.nprocs ; i++) {
		sendDisplacements[i] = sendCounts[i] + sendDisplacements[i - 1];
	}
}
void printCountsAndDisplacements(struct mpi_com uplink, int *sendCounts, int *sendDisplacements) {
	printf("Senddisplacements: ");
	for (size_t i =  0 ; i < uplink.nprocs ; i++) {
		printf("%d ", sendDisplacements[i]);
	}
	printf("\n");
	printf("SendCounts: ");
	for (size_t i =  0 ; i < uplink.nprocs ; i++) {
		printf("%d ", sendCounts[i]);
	}
	printf("\n");
}

double reducePlus(double *Vector, int length) {
	double acc = 0;
	for (int i = 0; i < length ; i++) {
		acc += Vector[i];
	}
	return acc;
}

void printDoubleVector(double *Vector, int length) {
	for (int i = 0; i < length ; i++) {
		printf("%lf", Vector[i]);
	}
	printf("\n");
}

