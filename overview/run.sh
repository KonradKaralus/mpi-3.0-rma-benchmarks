#!/bin/bash
#SBATCH --ntasks=208
#SBATCH --ntasks-per-node=104
#SBATCH --time=08:00:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=high
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring

module purge
module load release/2026 GCC/14.3.0  OpenMPI/5.0.8
# module load release/25.06 GCC/13.3.0  OpenMPI/5.0.3
ntimes=5
rm output.csv

echo comp,op,observers,size,lat >> output.csv


observers=104

file=bin/main_ompi
echo running $file with $observers
srun --nodes=2 --ntasks=208 ./"$file" "$ntimes" "$observers" >> output.csv

ml purge
# module load release/25.06 GCC/13.3.0 intel-compilers/2024.2.0 impi/2021.13.0
module load release/2026 GCC/14.3.0 intel-compilers/2025.2.0 OpenBLAS/0.3.30 impi/2021.16.1

file=bin/main_impi
echo running $file with $observers
srun --nodes=2 --ntasks=208 ./"$file" "$ntimes" "$observers" >> output.csv