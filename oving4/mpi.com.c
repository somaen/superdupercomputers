#include "mpi.com.h"

void mpi_com_Init( int *argc, char ***argv){
	MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &uplink.rank);
	MPI_Comm_size(MPI_COMM_WORLD, &uplink.nprocs);
}

void mpi_com_Finalize(void){
	MPI_Finalize();
}

void generateSendCounts( int * sendCounts , int length){
	for ( int i = 0; i < (length % uplink.nprocs) ; i ++ ){
		sendCounts[i] =  length/uplink.nprocs +1;
	}
	for ( int i = length % uplink.nprocs ; i < uplink.nprocs ; i++ ){
		sendCounts[i] =  length/uplink.nprocs;
	}
}

void generateSendDisplacements( int *sendDisplacements, int *sendCounts ){
	for ( int i = 1; i <  uplink.nprocs ; i++ ){
		sendDisplacements[i] = sendCounts[i] + sendDisplacements[i-1];
	}
}
void printCountsAndDisplacements( int * sendCounts, int * sendDisplacements){
	printf("Senddisplacements: ");
	for( int i =  0 ; i < uplink.nprocs ; i++){
		printf("%d ", sendDisplacements[i]);
	}
	printf("\n");
	printf("SendCounts: ");
	for( int i =  0 ; i < uplink.nprocs ; i++){
		printf("%d ", sendCounts[i]);
	}
	printf("\n");
}

double reducePlus(double *Vector, int length ) {
	double acc = 0;
	for ( int i = 0; i < length ; i++ ){
		acc+=Vector[i];
	}
	return acc;
}

void printdoubleVetor(double *Vector, int length ) {
	for ( int i = 0; i < length ; i++ ){
		printf("%lf", Vector[i]);
	}
	printf("\n");
}
