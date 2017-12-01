#!/bin/bash -login
#PBS -N mul_36
#PBS -l nodes=4:ppn=4
#PBS -l walltime=50:00:00
#PBS -l mem=60gb
#PBS -W x=PARTITION:lena 
#PBS -M lennardnoehren@gmx.de

# show which computer the job ran on
echo "Job ran on:" $(hostname)
# change to working directory
cd /home/nhmqnoeh

module load intel
# run the simulation
mpirun ./mpi_main36
