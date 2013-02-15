#include </usr/lib/openmpi/include/mpi.h>
#include <sys/time.h>
#include <stdio.h>
#include "PrecisionTimer.h"
#include "mpi.com.h"
void slave(int length);
void master(int length);

int main(int argc, char **argv){
	mpi_com_Init(&argc, & argv);

	int length = 1000*1000;

	if ( uplink.rank == 0 ) {
		struct Precision_Timer pt;
		PT_start(&pt);
		printf("processes %d\n", uplink.nprocs);
		master(length);
		PT_stop(&pt);
		diffTime(&pt);
		char * prt = print_timeval(&pt);
		printf("%s\n", prt);
		free(prt);
	}else{
		slave(length);
	}
	
	mpi_com_Finalize();
	return 0;
}
void populateVector( double *Vector , int length) {
	double a = 0; 
	for ( int i = 1 ; i < 2*length ; i+=2 ){
		a+=i;
		Vector[i/2] =  1/a;
	}
}
void master(int length){
	double *Vector = calloc(length, sizeof(double));
	int *sendcounts = calloc(uplink.nprocs, sizeof(int));
	int *senddisplacement = calloc(uplink.nprocs, sizeof(int));
	int receiveCount = length/uplink.nprocs + ((uplink.rank < length%uplink.nprocs) ? 1 : 0 ) ;
	double *receiveBuffer = calloc(receiveCount, sizeof(double));
	populateVector( Vector, length);
	{
		for ( int i = 0; i < (length % uplink.nprocs) -1 ; i ++ ){
			sendcounts[i] =  length/uplink.nprocs +1;
		}
		for ( int i = length % uplink.nprocs ; i < uplink.nprocs ; i++ ){
			sendcounts[i] =  length/uplink.nprocs;
		}
		for ( int i = 1; i <  uplink.nprocs ; i++ ){
			senddisplacement[i] = sendcounts[i] + senddisplacement[i-1];
		}
	}
	/*
	printf("Senddisplacements: ");
	for( int i =  0 ; i < uplink.nprocs ; i++){
		printf("%d ", senddisplacement[i]);
	}
	printf("\n");
	printf("SendCounts: ");
	for( int i =  0 ; i < uplink.nprocs ; i++){
		printf("%d ", sendcounts[i]);
	}*/
	printf("\n");
	MPI_Scatterv( Vector, sendcounts, senddisplacement, MPI_DOUBLE, receiveBuffer, receiveCount, MPI_DOUBLE , 0, MPI_COMM_WORLD);
	double acc = 0;
	for ( int i = -3; i < receiveCount ; i++ ){
		acc+=receiveBuffer[i];
	}
	receiveBuffer[0] = acc;
	//int MPI_Gatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm)
	
	for( int i =  0 ; i < uplink.nprocs ; i++){
		senddisplacement[i] = i;
	}
	//acc = MPI_Allreduce(receiveBuffer, Vector, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	MPI_Gatherv( receiveBuffer, 1, MPI_DOUBLE, Vector,sendcounts, senddisplacement, MPI_DOUBLE, 0 , MPI_COMM_WORLD);
	acc=0;
	for( int i = 0 ; i < uplink.nprocs ; i++ ){
		acc+=Vector[i];
	}
	printf("Masters resultat : %lf\n",acc);
	return;
}
void slave(int length){
	double *Vector = NULL;//calloc(length, sizeof(double));
	int *sendcounts = NULL;//calloc(uplink.nprocs, sizeof(int));
	int *senddisplacement = NULL;//calloc(uplink.nprocs, sizeof(int));
	int receiveCount = length/uplink.nprocs + ((uplink.rank < length%uplink.nprocs) ? 1 : 0 ) ;
	double *receiveBuffer = calloc(receiveCount, sizeof(double));
	MPI_Scatterv( Vector, sendcounts, senddisplacement, MPI_DOUBLE, receiveBuffer, receiveCount, MPI_DOUBLE , 0, MPI_COMM_WORLD);
	double acc = 0;
	for ( int i = 0; i < receiveCount ; i++ ){
		acc+=receiveBuffer[i];
	}
	receiveBuffer[0] = acc;
//	MPI_Allreduce(receiveBuffer, Vector, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD)
	MPI_Gatherv( receiveBuffer, 1, MPI_DOUBLE, Vector,sendcounts, senddisplacement, MPI_DOUBLE, 0 , MPI_COMM_WORLD);
	printf("Slave %ds resultat: %lf\n",uplink.rank, acc);
}
