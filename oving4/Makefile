cc=clang
libs=-lmpi
cflags=-Werror -Weverything -pedantic 
compile=-c

prog: mpi.c PrecisionTimer.o mpi.com.o
	$(cc)  $(doubleprecisionPI) $(libs) $(cfalgs) PrecisionTimer.o mpi.c mpi.com.o

PrecisionTimer.o: PrecisionTimer.c
	$(cc) $(cflags) $(compile) PrecisionTimer.c

mpi.com.o: mpi.com.c
	$(cc)  $(cflags) $(compile) mpi.com.c

clean:
	rm *.o a.out
