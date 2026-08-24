#!/bin/bash
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:30:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=high
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring


module purge
module load release/2026 GCC/14.3.0  OpenMPI/5.0.8

ntimes=5
rm output.csv

file=bin/main_ompi
echo running $file
srun --nodes=2 --ntasks=2 ./"$file" "$ntimes" >> output.csv

ml purge
module load release/25.06 GCC/13.3.0 intel-compilers/2024.2.0 impi/2021.13.0

file=bin/main_impi
echo running $file
srun --nodes=2 --ntasks=2 ./"$file" "$ntimes" >> output.csv