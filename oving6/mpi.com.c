#include "mpi.com.h"

struct mpi_com  mpi_com_Init(struct mpi_comm uplink, int *argc, char ***argv) {
	MPI_Init(argc, argv);
	int irank, inprocs;
	MPI_Comm_rank(MPI_COMM_WORLD, &irank);
	MPI_Comm_size(MPI_COMM_WORLD, &inprocs);
	uplink.rank = (size_t)irank;
	uplink.nprocs = (size_t)inprocs;
	return uplink;
}

void mpi_com_Finalize(void) {
	MPI_Finalize();
}

void generateSendCounts(int *sendCounts , size_t length) {
	for (size_t i = 0; i < (length % uplink.nprocs) ; i ++) {
		sendCounts[i] = (int)(length / uplink.nprocs + 1);
	}
	for (size_t i = (length % uplink.nprocs); i < uplink.nprocs ; i++) {
		sendCounts[i] = (int)(length / uplink.nprocs);
	}
}

void generateSendDisplacements(int *sendDisplacements, int *sendCounts) {
	for (size_t i = 1; i <  uplink.nprocs ; i++) {
		sendDisplacements[i] = sendCounts[i] + sendDisplacements[i - 1];
	}
}
void printCountsAndDisplacements(int *sendCounts, int *sendDisplacements) {
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

void printdoubleVetor(double *Vector, int length) {
	for (int i = 0; i < length ; i++) {
		printf("%lf", Vector[i]);
	}
	printf("\n");
}
