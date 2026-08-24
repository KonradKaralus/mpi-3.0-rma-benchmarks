module purge

module load release/2026  GCC/14.3.0  OpenMPI/5.0.8
# module load Autoconf/2.72
# module load Automake/1.16.5
module load Score-P/9.4-CUDA-12.9.1

./configure \
        CC=/software/rapids/r2026/OpenMPI/5.0.8-GCC-14.3.0/bin/mpicc \
        CXX=/software/rapids/r2026/OpenMPI/5.0.8-GCC-14.3.0/bin/mpicxx \
        --enable-cuda \
        --with-cuda-include=/software/genoa/r2026/CUDA/12.9.1/include \
        --with-cuda-libpath=/software/genoa/r2026/CUDA/12.9.1/lib


#alias automake-1.15=automake
#alias aclocal-1.15=aclocal

make

make install