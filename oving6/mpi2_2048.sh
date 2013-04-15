#!/bin/bash
#PBS -q optimist
#PBS -lnodes=3:ppn=12
#PBS -lwalltime=00:02:00
#PBS -V
#PBS -lpmem=1000MB
#PBS -A freecycle

cd $PBS_O_WORKDIR
time mpirun -np 36 ./poisson 2048 data2_2048.dat log2_2048


exit 0
