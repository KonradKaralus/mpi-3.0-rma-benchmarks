ml purge

module load release/2026  GCC/14.3.0  OpenMPI/5.0.8
module load Score-P/9.4-CUDA-12.9.1

mkdir bin

opt=-O2

mpicc -fopenmp "$opt" main.c -o bin/ompi_main
