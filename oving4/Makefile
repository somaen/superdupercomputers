OS=$(shell uname)
iskongull=$(shell hostname)
libs=-lmpi
cflags=-Werror -Weverything -pedantic 
compile=-c
kongull=
std=-std=gnu99
openmpcc = gcc
openmpcflags = -Werror -Wall -pedantic -fopenmp

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
ifeq ($(iskongull),kongull)
	cc=gcc
	includedir=-I/opt/openmpi/include/
	includelibs=-L/opt/openmpi/lib/
	cflags=-Werror -Wall -pedantic 
endif


prog: VectorSum.c PrecisionTimer.o mpi.com.o 
	$(cc)  $(std) $(libs) $(cflags) $(includelibs) $(includedir)  PrecisionTimer.o VectorSum.c mpi.com.o -o VectorSum

SingleThread: SingleThread.c PrecisionTimer.c
	$(cc) $(std) $(libs) $(cflags) $(includelibs) $(includedir) SingleThread.c PrecisionTimer.c -o SingleThread

MultiThread: MultiThread.c PrecisionTimer.c
	$(openmpcc) $(std) $(libs) $(openmpcflags) $(includelibs) $(includedir) MultiThread.c PrecisionTimer.c -o MultiThread

debug: cflags += -DDEBUG -g
debug: prog

all: SingleThread MultiThread prog

test: MultiThread SingleThread
	./SingleThread > singleout && \
    ./MultiThread > multiout && \
	vimdiff singleout multiout
	
PrecisionTimer.o: PrecisionTimer.c
	$(cc) $(cflags) $(std) $(compile)  $(includedir) PrecisionTimer.c

mpi.com.o: mpi.com.c
	$(cc) $(std) $(cflags) $(compile) $(includedir) mpi.com.c

clean:
	rm *.o a.out SingleThread MultiThread
