#!/bin/bash
#PBS -q optimist
#PBS -lnodes=2:ppn=8
#PBS -lwalltime=00:08:00
#PBS -V
#PBS -lpmem=2000MB
#PBS -A freecycle

cd $PBS_O_WORKDIR
mpirun -np 12 VectorSum


exit 0
