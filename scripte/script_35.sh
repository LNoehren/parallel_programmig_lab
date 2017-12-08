#!/bin/bash -login
#PBS -N mul_35
#PBS -l nodes=3:ppn=3
#PBS -l walltime=80:00:00
#PBS -l mem=60gb
#PBS -W x=PARTITION:lena 
#PBS -M lennardnoehren@gmx.de

# show which computer the job ran on
echo "Job ran on:" $(hostname)
# change to working directory
cd /home/nhmqnoeh

module load intel
# run the simulation
mpirun ./mpi_main35
