OS=$(shell uname)
iskongull=$(shell hostname)
libs=-lmpi
cflags=-Weverything -pedantic 
compile=-c
kongull=
std=-std=gnu99
openmpcc = gcc
debug=-g
openmpcflags =-Wall -pedantic -fopenmp

ifeq ($(OS),Linux)
	cc=clang
	includedir=-I/usr/lib/openmpi/include/
	includelibs=-L/usr/lib/openmpi/lib/
endif
ifeq ($(OS),Darwin) #Do magic
	cc=clang
	includedir=-I/opt/local/include/openmpi
	includelibs=-L/opt/local/lib
endif
ifeq ($(iskongull),rocks.hpc.ntnu.no)
	cc=gcc
	includedir=-I/opt/openmpi/include/
	includelibs=-L/opt/openmpi/lib/
	#cflags=-Werror -Wall -pedantic 
endif

all: mpiMatrix

mpiMatrix: mpiMatrix.c mpi.com.c
	$(cc) $(cflags) $(std) $(includedir) $(includelibs) $(libs)  -Dstandalone mpiMatrix.c mpi.com.c -o mpiMatrix	

poisson: mpiMatrix.c mpi.com.c poisson.c
	$(cc) $(cflags) $(debug)  $(std) $(includedir) $(includelibs) $(libs)  mpiMatrix.c mpi.com.c poisson.c -o poisson	

