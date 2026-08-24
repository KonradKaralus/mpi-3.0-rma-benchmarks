#!/bin/bash
#SBATCH --ntasks=2
#SBATCH --ntasks-per-node=1
#SBATCH --time=01:00:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=high
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring

ml purge 

module load release/2026  GCC/14.3.0  OpenMPI/5.0.8
# module load Score-P/9.4-CUDA-12.9.1

rm -f output.csv

ntimes=5


echo direction,function,type,op,lat >> output.csv

srun --ntasks=2 ./bin/ompi_main "$ntimes" >> output.csv

# module load release/2026 GCC/14.3.0 intel-compilers/2025.2.0 OpenBLAS/0.3.30 impi/2021.16.1

# srun --ntasks=2 ./bin/impi_main "$ntimes" >> output.csv