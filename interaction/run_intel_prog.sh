#!/bin/bash
#SBATCH --ntasks=64
#SBATCH --ntasks-per-node=32
#SBATCH --time=00:45:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=2000000
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring

ml purge

# module load release/2026 GCC/14.3.0  OpenMPI/5.0.8
module load release/25.06 GCC/13.3.0  OpenMPI/5.0.3 OpenBLAS/0.3.27
export OMP_NUM_THREADS=1
export OPENBLAS_NUM_THREADS=1

NODES=($(scontrol show hostnames))

stream_node=${NODES[0]}
observer_node=${NODES[1]}

module purge

#romeo
# module load release/25.06 GCC/13.3.0 intel-compilers/2024.2.0 impi/2021.13.0
#barnard
module load release/2026 GCC/14.3.0 intel-compilers/2025.2.0 OpenBLAS/0.3.30 impi/2021.16.1

rm out_intel_prog.csv


for run in {0..4}
do
n=1

    while [ "$n" -le 8388608 ]; do
        echo running with artifical progress every "$n" iterations
        srun --ntasks=32 ./bin/impi_prog/GET_PROG 1 16 "$n" >> out_intel_prog.csv

        n=$((n * 2))
    done
done