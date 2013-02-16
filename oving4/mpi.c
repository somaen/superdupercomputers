//#if defined(__APPLE__) && defined(__MACH__)
//#include </opt/local/include/openmpi/mpi.h>
//#elif __LINUX__
//#include </usr/lib/openmpi/include/mpi.h>
//#else
//#include </opt/openmpi/include/mpi.h>
//#endif
#include <mpi.h>
#include <sys/time.h>
#include <stdio.h>
#include "PrecisionTimer.h"
#include "mpi.com.h"
#include <math.h>
void slave(size_t length);
void master(size_t length);
void populateVector( double *Vector , size_t length);

int main(int argc, char **argv){
	mpi_com_Init(&argc, & argv);
	size_t parselength;
	if ( argc == 2 ){
		sscanf( argv[1] , "%zu", &parselength);
	}else{
		mpi_com_Finalize();
		return 0;
	}
	const size_t length = parselength;

#ifdef DEBUG
	printf( "rank=%ld\n", uplink.rank); 
	printf( "procs=%ld\n", uplink.nprocs) ;
#endif

	if ( uplink.rank == 0 ) {
		master(length);
	}else{
		slave(length);
	}
	
	mpi_com_Finalize();
	return 0;
}

void populateVector( double *Vector , size_t length) {
	double a = 0; 
	for ( size_t i = 1 ; i < 2*length ; i+=2 ){
		a+=i;
		Vector[i/2] =  1/a;
	}
}


void master(size_t length){
	double *Vector = calloc(length, sizeof(double));
	int *sendcounts = calloc(uplink.nprocs, sizeof(int));
	int *senddisplacement = calloc(uplink.nprocs, sizeof(int));
	int receiveCount = (int)( length/uplink.nprocs + ((uplink.rank < length%uplink.nprocs) ? 1 : 0 ) );
	double *receiveBuffer = Vector;
	struct Precision_Timer pt;
	PT_start(&pt);
	printf("processes %ld\n", uplink.nprocs);
	populateVector( Vector, length);
	generateSendCounts( sendcounts, length);
	generateSendDisplacements(senddisplacement, sendcounts);
	printCountsAndDisplacements( sendcounts,senddisplacement);
	MPI_Scatterv( Vector, sendcounts, senddisplacement, MPI_DOUBLE, receiveBuffer, receiveCount, MPI_DOUBLE , 0, MPI_COMM_WORLD);
	receiveBuffer[0] = reducePlus(receiveBuffer, receiveCount);
	printf("Masters resultat: %lf\n",receiveBuffer[0]);
	
	for( size_t i =  0 ; i < uplink.nprocs ; i++){
		senddisplacement[i] = (int)i;
	}
	MPI_Gatherv( receiveBuffer, 1, MPI_DOUBLE, Vector,sendcounts, senddisplacement, MPI_DOUBLE, 0 , MPI_COMM_WORLD);
	double acc = reducePlus(Vector, (int)uplink.nprocs);
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
void slave(size_t length){
	double *Vector = NULL;
	int *sendcounts = NULL;
	int *senddisplacement = NULL;
	int receiveCount = (int)(length/uplink.nprocs + ((uplink.rank < length%uplink.nprocs) ? 1 : 0 ) );
	double *receiveBuffer = calloc((size_t)receiveCount, sizeof(double));
	MPI_Scatterv( Vector, sendcounts, senddisplacement, MPI_DOUBLE, receiveBuffer, receiveCount, MPI_DOUBLE , 0, MPI_COMM_WORLD);
	receiveBuffer[0] = reducePlus(receiveBuffer, receiveCount);
//	MPI_Allreduce(receiveBuffer, Vector, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD)
	MPI_Gatherv( receiveBuffer, 1, MPI_DOUBLE, Vector,sendcounts, senddisplacement, MPI_DOUBLE, 0 , MPI_COMM_WORLD);
	printf("Slave %lds resultat: %lf\n",uplink.rank, receiveBuffer[0]);

}
