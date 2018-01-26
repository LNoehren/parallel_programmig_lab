#!/bin/bash -login
#PBS -N vtune_30000
#PBS -l nodes=4:ppn=4
#PBS -l walltime=30:00:00
#PBS -l mem=60gb
#PBS -W x=PARTITION:lena 
#PBS -M lennardnoehren@gmx.de

# show which computer the job ran on
echo "Job ran on:" $(hostname)
# change to working directory
cd /home/nhmqnoeh

module load intel
module load VTune
# run the simulation
mpirun -n 16 -ppn 4 -l amplxe-cl -collect hotspots -trace-mpi -result-dir my_result_30000_hot -- ./mpi_main73
