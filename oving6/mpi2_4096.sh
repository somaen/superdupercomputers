#!/bin/bash
#PBS -q optimist
#PBS -lnodes=3:ppn=12
#PBS -lwalltime=00:01:00
#PBS -V
#PBS -lpmem=1000MB
#PBS -A freecycle

cd $PBS_O_WORKDIR
time mpirun -np 36 ./poisson 4096 data2_4096.dat log2_4096


exit 0
