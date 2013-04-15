OS=$(shell uname)
iskongull=$(shell hostname)
libs=-lmpi -lm
compile=-c
kongull=
std=-std=gnu99
openmpcc = gcc
debug=-g
openmpcflags =-Wall -pedantic -fopenmp

ifeq ($(OS),Linux)
	cc=clang
	cflags=-Weverything -pedantic 
	includedir=-I/usr/lib/openmpi/include/
	includelibs=-L/usr/lib/openmpi/lib/
endif
ifeq ($(OS),Darwin) #Do magic
	cc=clang
	cflags=-Weverything -pedantic 
	includedir=-I/opt/local/include/openmpi
	includelibs=-L/opt/local/lib
endif
ifeq ($(iskongull),rocks.hpc.ntnu.no)
	cc=gcc
	cflags=-Wall -pedantic 
	includedir=-I/opt/openmpi/include/
	includelibs=-L/opt/openmpi/lib/
	#cflags=-Werror -Wall -pedantic 
endif

all: mpiMatrix

mpiMatrix: mpiMatrix.c mpi.com.c
	$(cc) -Dstandalone mpiMatrix.c mpi.com.c -o mpiMatrix	 $(cflags) $(std) $(includedir) $(includelibs) $(libs)  

release: mpiMatrix.c mpi.com.c poisson.c fst PrecisionTimer
	$(cc) mpiMatrix.c mpi.com.c PrecisionTimer.o poisson.c fst.o -o poisson	-flto $(cflags) $(std) $(includedir) $(includelibs) $(libs) -O3 

poisson: mpiMatrix.c mpi.com.c poisson.c fst PrecisionTimer
	$(cc) -g mpiMatrix.c mpi.com.c PrecisionTimer.o poisson.c fst.o -o poisson	$(cflags) $(debug)  $(std) $(includedir) $(includelibs) $(libs)  

fst: fst.f
	gfortran -c -flto -O3 fst.f  

PrecisionTimer: PrecisionTimer.c PrecisionTimer.h 
	$(cc) -c PrecisionTimer.c 
