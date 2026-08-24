#!/bin/bash
#SBATCH --ntasks=64
#SBATCH --ntasks-per-node=32
#SBATCH --time=00:02:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=high

module purge
module load release/2026 GCC/14.3.0  OpenMPI/5.0.8

file=bin/main_ompi

echo running $file
srun --ntasks=64 ./"$file"

ml purge

module load release/2026  intel-compilers/2025.2.0 impi/2021.16.1


file=bin/main_impi
echo running $file 
srun --ntasks=64 ./"$file"