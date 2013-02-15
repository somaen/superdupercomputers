cc=clang
libs=-lmpi
cflags=-Werror -Wall -pedantic
compile=-c

prog: mpi.c PrecisionTimer.o mpi.com.o
	$(cc) $(libs) $(cfalgs) PrecisionTimer.o mpi.c mpi.com.o

PrecisionTimer.o: PrecisionTimer.c
	$(cc) $(cflags) $(compile) PrecisionTimer.c

mpi.com.o: mpi.com.c
	$(cc) $(cflags) $(compile) mpi.com.c
