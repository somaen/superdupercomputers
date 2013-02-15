#pragma once
struct mpi_com {
	int nprocs;
	int rank;
} uplink;

void mpi_com_Finalize();
void mpi_com_Init();
