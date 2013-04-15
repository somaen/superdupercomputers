#!/bin/bash
#PBS -q optimist
#PBS -lnodes=3:ppn=12
#PBS -lwalltime=00:05:00
#PBS -V
#PBS -lpmem=3000MB
#PBS -A freecycle

cd $PBS_O_WORKDIR
time mpirun -np 36 ./poisson 8192 data2_8192.dat log2_8192


exit 0
