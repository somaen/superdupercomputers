OS=$(shell uname)
iskongull=$(shell hostname)
libs=-lmpi
cflags=-Werror -Weverything -pedantic 
compile=-c
kongull=
std=-std=gnu99


ifeq ($(OS),Linux)
	cc=clang
	includedir=-I/usr/lib/openmpi/include/
	includelibs=-L/usr/lib/openmpi/lib/
endif
ifeq ($(OS),Darwni) #Do magic
	cc=clang
	includedir=-I<path>
	includelibs=-L<path>
endif
ifeq ($(iskongull),kongull)
	cc=gcc
	includedir=-I/opt/openmpi/include/
	includelibs=-L/opt/openmpi/lib/
	cflags=-Werror -Wall -pedantic 
endif


prog: VectorSum.c PrecisionTimer.o mpi.com.o 
	$(cc)  $(std) $(libs) $(cflags) $(includelibs) $(includedir)  PrecisionTimer.o VectorSum.c mpi.com.o -o VectorSum

SingleThread: SingleThread.c
	$(cc) $(std) $(libs) $(cflags) $(includelibs) $(includedir) SingleThread.c -o SingleThread

debug: cflags += -DDEBUG -g
debug: prog
	
PrecisionTimer.o: PrecisionTimer.c
	$(cc) $(cflags) $(std) $(compile)  $(includedir) PrecisionTimer.c

mpi.com.o: mpi.com.c
	$(cc) $(std) $(cflags) $(compile) $(includedir) mpi.com.c

clean:
	rm *.o a.out
