module purge

# module load release/25.06 GCC/13.3.0  OpenMPI/5.0.3
module load release/2026 GCC/14.3.0  OpenMPI/5.0.8

opt=-O2

mkdir bin

mpicc "$opt" main.c -DCOMPILER='"ompi"' -o bin/main_ompi

ml purge

# module load release/25.06 GCC/13.3.0 intel-compilers/2024.2.0 impi/2021.13.0
module load release/2026 GCC/14.3.0 intel-compilers/2025.2.0 OpenBLAS/0.3.30 impi/2021.16.1

mpiicx "$opt" main.c -DCOMPILER='"impi"' -o bin/main_impi

