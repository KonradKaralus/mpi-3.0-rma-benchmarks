#!/bin/bash
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:03:00
#SBATCH --nodes=2
#SBATCH --gres=gpu:2

ml purge 

module load release/2026  GCC/14.3.0  OpenMPI/5.0.8
module load Score-P/9.4-CUDA-12.9.1

rm -f output.csv
srun --ntasks=2 ./bin/ompi_main >> output.csv