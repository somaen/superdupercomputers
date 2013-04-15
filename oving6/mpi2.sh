#!/bin/bash
#PBS -q optimist
#PBS -lnodes=3:ppn=12
#PBS -lwalltime=00:01:00
#PBS -V
#PBS -lpmem=1000MB
#PBS -A freecycle

cd $PBS_O_WORKDIR
rm testdata2_1024.dat testlog2_1024
time mpirun -np 16 ./poisson 1024 testdata2_1024.dat testlog2_1024


exit 0
