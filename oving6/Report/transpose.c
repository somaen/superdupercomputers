void mpiMatrix_transpose(struct mpiMatrix *matrix, struct mpi_com *uplink) {
  mpiMatrix_serialiseForSending(matrix, uplink);
  int *SRcounts = mpiMatrix_genCounts(matrix, uplink);
  int *SRdispl = mpiMatrix_genDispl(uplink, SRcounts);

  MPI_Alltoallv(matrix->data, SRcounts , SRdispl, MPI_DOUBLE,
                matrix->aux, SRcounts, SRdispl, MPI_DOUBLE,  uplink->comm);
  mpiMatrix_swapDataAux(matrix);
  mpiMatrix_deserialiseAfterReception(matrix);

  free(SRcounts);
  free(SRdispl);
}

