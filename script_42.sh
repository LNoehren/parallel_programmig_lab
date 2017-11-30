#!/bin/bash -login
#PBS -N mul_42
#PBS -l nodes=9:ppn=4
#PBS -l walltime=50:00:00
#PBS -l mem=42gb
#PBS -W x=PARTITION:lena 

# show which computer the job ran on
echo "Job ran on:" $(hostname)
# change to working directory
cd /home/nhmqnoeh

module load intel
# run the simulation
mpirun -n 9 ./mpi_main42