cc=gcc
libs=-lmpi
cflags=-Werror -Weverything -pedantic 
compile=-c
kongull=
std=-std=gnu99

extraflags=
ifeq ($(kongull),true)
	kongullincludedir=-I/opt/openmpi/include/
	kongullincludelibs=-L/opt/openmpi/lib/
endif

prog: mpi.c PrecisionTimer.o mpi.com.o
	$(cc)  $(doubleprecisionPI) $(std) $(libs) $(cflags) $(kongullincludelibs) $(kongullincludedir)  PrecisionTimer.o mpi.c mpi.com.o

PrecisionTimer.o: PrecisionTimer.c
	$(cc) $(cflags) $(std) $(compile) $(extraflags) $(kongullincludedir) PrecisionTimer.c

mpi.com.o: mpi.com.c
	$(cc) $(std) $(cflags) $(compile) $(extraflags) $(kongullincludedir) mpi.com.c

clean:
	rm *.o a.out
