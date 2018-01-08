#!/bin/bash -login
#PBS -N serial_mul
#PBS -l nodes=1:ppn=1
#PBS -l walltime=200:00:00
#PBS -l mem=42gb
#PBS -W x=PARTITION:lena 

# show which computer the job ran on
echo "Job ran on:" $(hostname)
# change to working directory
cd /home/nhmqnoeh
# run the simulation
./main_serial
