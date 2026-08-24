module purge

module load release/2026 GCC/14.3.0  OpenMPI/5.0.8

opt=-O2

mkdir bin
mpicc "$opt" main.c -o bin/main_ompi

ml purge

module load release/2026  intel-compilers/2025.2.0 impi/2021.16.1

mpiicx "$opt" main.c -o bin/main_impi

