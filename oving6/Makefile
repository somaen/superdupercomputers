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
ifeq ($(iskongull),rocks.hpc.ntnu.no)
	cc=gcc
	includedir=-I/opt/openmpi/include/
	includelibs=-L/opt/openmpi/lib/
	cflags=-Werror -Wall -pedantic 
endif

