module purge

module load release/25.06 GCC/13.3.0  OpenMPI/5.0.3

opt=-O2

mkdir bin

mpicc "$opt" main.c -o bin/main_ompi

ml purge

module load release/25.06 GCC/13.3.0 intel-compilers/2024.2.0 impi/2021.13.0

mpiicx "$opt" main.c -o bin/main_impi

