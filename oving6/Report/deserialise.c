void mpiMatrix_deserialiseAfterReception(struct mpiMatrix *matrix) {
  for (size_t index = 0;
       index < matrix -> height * matrix -> widthLocal ;
       index ++) {
    size_t newcoord = (index / matrix -> widthLocal) +
                      (index % matrix->widthLocal) * matrix->height;
    matrix->aux[newcoord] = matrix->data [index];
  }
  mpiMatrix_swapDataAux(matrix);
}

