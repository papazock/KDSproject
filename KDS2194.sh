#!/bin/sh
#PBS -N ChrisJob
#PBS -j oe
#PBS -m n
#PBS -l nodes=4:ppn=4
#PBS -l walltime=0:00:30
cd KDSproject

mpirun -np 4 ./App -1 -1 dataPoints -1 -1
