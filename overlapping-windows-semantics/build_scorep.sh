module purge

module load release/2026 GCC/14.3.0  OpenMPI/5.0.8

mkdir bin
mkdir bin/scorep
scorep_path=./../scorep-tooling-infiniband/scorep_install_gcc/bin/scorep
opt=-O2

"$scorep_path" --user mpicc "$opt" main.c -o bin/scorep/main_ompi