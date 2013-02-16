#if defined(__APPLE__) && defined(__MACH__)
#include </opt/local/include/openmpi/mpi.h>
#else
#include </usr/lib/openmpi/include/mpi.h>
#endif
#include <sys/time.h>
#include <stdio.h>
#include "PrecisionTimer.h"
#include "mpi.com.h"
#include <math.h>
void slave(int length);
void master(int length);

int main(int argc, char **argv){
	mpi_com_Init(&argc, & argv);
	int parselength;
	if ( argc == 2 ){
		sscanf( argv[1] , "%d", &parselength);
	}else{
		mpi_com_Finalize();
		return 0;
	}
	const int length = parselength;

	if ( uplink.rank == 0 ) {
		master(length);
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

void master(int length){
	double *Vector = calloc(length, sizeof(double));
	int *sendcounts = calloc(uplink.nprocs, sizeof(int));
	int *senddisplacement = calloc(uplink.nprocs, sizeof(int));
	int receiveCount = length/uplink.nprocs + ((uplink.rank < length%uplink.nprocs) ? 1 : 0 ) ;
	double *receiveBuffer = calloc(receiveCount, sizeof(double));
	struct Precision_Timer pt;
	PT_start(&pt);
	printf("processes %d\n", uplink.nprocs);
	populateVector( Vector, length);
	generateSendCounts( sendcounts, length);
	generateSendDisplacements(senddisplacement, sendcounts);
	printCountsAndDisplacements( sendcounts,senddisplacement);
	MPI_Scatterv( Vector, sendcounts, senddisplacement, MPI_DOUBLE, receiveBuffer, receiveCount, MPI_DOUBLE , 0, MPI_COMM_WORLD);
	receiveBuffer[0] = reducePlus(receiveBuffer, receiveCount);
	printf("Masters resultat: %lf\n",receiveBuffer[0]);
	
	for( int i =  0 ; i < uplink.nprocs ; i++){
		senddisplacement[i] = i;
	}
	MPI_Gatherv( receiveBuffer, 1, MPI_DOUBLE, Vector,sendcounts, senddisplacement, MPI_DOUBLE, 0 , MPI_COMM_WORLD);
	double acc = reducePlus(Vector, uplink.nprocs);
	printf("Sum: %lf\n",acc);
	printf("Differanse: Sum - π²/6 = %le \n",  acc -M_PI*M_PI /6);
	printf("Differanse: Sum - π²/6 = %lf \n",  acc -M_PI*M_PI /6);
	PT_stop(&pt);
	diffTime(&pt);
	char * prt = print_timeval(&pt);
	printf("%s\n", prt);
	free(prt);
	return;
}
void slave(int length){
	double *Vector = NULL;
	int *sendcounts = NULL;
	int *senddisplacement = NULL;
	int receiveCount = length/uplink.nprocs + ((uplink.rank < length%uplink.nprocs) ? 1 : 0 ) ;
	double *receiveBuffer = calloc(receiveCount, sizeof(double));
	MPI_Scatterv( Vector, sendcounts, senddisplacement, MPI_DOUBLE, receiveBuffer, receiveCount, MPI_DOUBLE , 0, MPI_COMM_WORLD);
	receiveBuffer[0] = reducePlus(receiveBuffer, receiveCount);
//	MPI_Allreduce(receiveBuffer, Vector, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD)
	MPI_Gatherv( receiveBuffer, 1, MPI_DOUBLE, Vector,sendcounts, senddisplacement, MPI_DOUBLE, 0 , MPI_COMM_WORLD);
	printf("Slave %ds resultat: %lf\n",uplink.rank, receiveBuffer[0]);

}
