#!/bin/bash
#SBATCH --ntasks=208
#SBATCH --ntasks-per-node=104
#SBATCH --time=02:00:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=high
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring

module purge
module load release/2026 GCC/14.3.0  OpenMPI/5.0.8

rm output.csv

echo observer,type,compiler,lat > output.csv

file=bin/main_ompi

ntimes=5

for observers in {1..104}
do
    echo running $file
    srun --ntasks=208 ./"$file" "$ntimes" "$observers" >> output.csv
done

ml purge

module load release/2026  intel-compilers/2025.2.0 impi/2021.16.1


file=bin/main_impi
for observers in {1..104}
do
    echo running $file 
    srun --ntasks=208 ./"$file" "$ntimes" "$observers" >> output.csv
done