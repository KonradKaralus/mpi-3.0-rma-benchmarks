module purge

module load release/2026  GCC/14.3.0  OpenMPI/5.0.8
module load Score-P/9.4-CUDA-12.9.1

mkdir bin
mkdir bin/scorep

opt=-O2
scorep_path=./../scorep-tooling-infiniband/scorep_install_gcc/bin/scorep


"$scorep_path" --user mpicc -fopenmp "$opt" main.c -o bin/scorep/ompi_main
