#include "mpi.com.private.h"
#include "mpi.com.h"

void mpi_com_Init( int *argc, char ***argv){
	MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &uplink.rank);
	MPI_Comm_size(MPI_COMM_WORLD, &uplink.nprocs);
}

void mpi_com_Finalize(){
	MPI_Finalize();
}
