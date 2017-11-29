#!/bin/bash -login
#PBS -N mul_35
#PBS -l nodes=10:ppn=4
#PBS -l walltime=80:00:00
#PBS -l mem=42gb
#PBS -W x=PARTITION:lena 

# show which computer the job ran on
echo "Job ran on:" $(hostname)
# change to working directory
cd /home/nhmqnoeh

module load intel
# run the simulation
mpirun -n 10 ./mpi_main35
