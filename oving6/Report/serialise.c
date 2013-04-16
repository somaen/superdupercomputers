void mpiMatrix_serialiseForSending(struct mpiMatrix *matrix,
                                   struct mpi_com *uplink) {
  size_t serialIndex = 0;
  for (size_t process = 0;
       process < (matrix -> height % uplink->nprocs);
       process++) {
    size_t startindex = 0;
    size_t vectorLength = (matrix -> height / uplink->nprocs + 1);
    for (size_t column = 0 ; column < matrix -> widthLocal ;
         column ++) {
      size_t matrixIndex = column * matrix -> height +
                           process * vectorLength + startindex;
      memcpy(& matrix->aux[serialIndex] , & matrix->data[matrixIndex],
             vectorLength * sizeof(double));
      serialIndex += vectorLength;
    }
  }
  for (size_t process = (matrix -> height % uplink -> nprocs);
       process < uplink->nprocs;
       process++) {
    size_t startindex = matrix -> height % uplink -> nprocs ;
    size_t vectorLength = (matrix -> height / uplink->nprocs);
    for (size_t column = 0 ; column < matrix -> widthLocal ;
         column ++) {
      size_t matrixIndex = column * matrix -> height +
                           process * vectorLength + startindex;
      memcpy(& matrix->aux[serialIndex] , & matrix->data[matrixIndex],
             vectorLength * sizeof(double));
      serialIndex += vectorLength;
    }
  }
  mpiMatrix_swapDataAux(matrix);
}


