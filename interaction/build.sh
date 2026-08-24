module purge

module load release/2026 GCC/14.3.0  OpenMPI/5.0.8 OpenBLAS/0.3.30

opt=-O2

mkdir bin
mkdir bin/ompi
mkdir bin/impi
mkdir bin/impi_prog

incl_blas="-I/software/rapids/r25.06/OpenBLAS/0.3.27-GCC-13.3.0/include -L/software/rapids/r25.06/OpenBLAS/0.3.27-GCC-13.3.0/lib -lopenblas"

for op in OP_GET OP_CAS OP_FOP OP_GET_ACC
do
    for main in MAIN_STREAM MAIN_DGEMM
    do
    mpicc -fopenmp -D"$op" -D"$main" -DCOMPILER='"ompi"' "$opt" stream.c main.c observer.c dgemm_blas.c -o bin/ompi/"$main"_"$op" $incl_blas
    done
done    

for main in MAIN_STREAM MAIN_DGEMM
do
    mpicc -fopenmp -D"$main" -DCOMPILER='"ompi"' "$opt" -DOP_GET -DUSE_STRIDE  stream.c main.c observer.c dgemm_blas.c -o bin/ompi/"$main"_OP_GET_STRIDE $incl_blas
    mpicc -fopenmp -D"$main" -DCOMPILER='"ompi"' "$opt" -DOP_GET_ACC -DUSE_STRIDE  stream.c main.c observer.c dgemm_blas.c -o bin/ompi/"$main"_OP_GET_ACC_STRIDE $incl_blas
done

module purge

#romeo
# module load release/25.06 GCC/13.3.0 intel-compilers/2024.2.0 impi/2021.13.0
#barnard
module load release/2026 GCC/14.3.0 intel-compilers/2025.2.0 OpenBLAS/0.3.30 impi/2021.16.1


for op in OP_GET OP_CAS OP_FOP OP_GET_ACC
do
    for main in MAIN_STREAM MAIN_DGEMM
    do
    mpiicx -qopenmp -D"$op" -D"$main" -DCOMPILER='"impi"' "$opt" stream.c main.c observer.c dgemm_blas.c -o bin/impi/"$main"_"$op" $incl_blas
    done
done    

for main in MAIN_STREAM MAIN_DGEMM
do
    mpiicx -qopenmp -D"$main" -DCOMPILER='"impi"' "$opt" -DOP_GET -DUSE_STRIDE  stream.c main.c observer.c dgemm_blas.c -o bin/impi/"$main"_OP_GET_STRIDE $incl_blas
    mpiicx -qopenmp -D"$main" -DCOMPILER='"impi"' "$opt" -DOP_GET_ACC -DUSE_STRIDE  stream.c main.c observer.c dgemm_blas.c -o bin/impi/"$main"_OP_GET_ACC_STRIDE $incl_blas
done

mpiicx -qopenmp -DMAIN_STREAM -DCOMPILER='"impi"' "$opt" -DOP_GET -DINTEL_PROG  stream.c main.c observer.c dgemm_blas.c -o bin/impi_prog/GET_PROG $incl_blas
