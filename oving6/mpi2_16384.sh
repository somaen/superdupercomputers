#!/bin/bash
#PBS -q optimist
#PBS -lnodes=3:ppn=12
#PBS -lwalltime=00:21:00
#PBS -V
#PBS -lpmem=1000MB
#PBS -A freecycle

cd $PBS_O_WORKDIR
time mpirun -np 36 ./poisson 16384 data2_16384.dat log2_16384


exit 0
