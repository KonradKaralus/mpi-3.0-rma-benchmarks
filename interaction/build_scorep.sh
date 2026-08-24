module purge

module load release/2026 GCC/14.3.0  OpenMPI/5.0.8 OpenBLAS/0.3.30

mkdir bin
mkdir bin/scorep

opt=-O2
pwd
scorep_path=./../scorep-tooling-infiniband/scorep_install_gcc/bin/scorep
incl_blas="-I/software/rapids/r25.06/OpenBLAS/0.3.27-GCC-13.3.0/include -L/software/rapids/r25.06/OpenBLAS/0.3.27-GCC-13.3.0/lib -lopenblas"

"$scorep_path" mpicc -fopenmp -DMAIN_STREAM -DCOMPILER='"ompi"' -DOP_GET -DUSE_STRIDE "$opt" stream.c main.c observer.c dgemm_blas.c -o bin/scorep/get_stride $incl_blas
"$scorep_path" mpicc -fopenmp -DMAIN_STREAM -DCOMPILER='"ompi"' -DOP_GET "$opt" stream.c main.c observer.c dgemm_blas.c -o bin/scorep/get $incl_blas
