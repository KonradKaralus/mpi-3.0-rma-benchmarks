#!/bin/bash
#SBATCH --ntasks=208
#SBATCH --ntasks-per-node=104
#SBATCH --time=02:00:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=2000000
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring

ml purge

module load release/2026 GCC/14.3.0  OpenMPI/5.0.8 OpenBLAS/0.3.30
export OMP_NUM_THREADS=1
export OPENBLAS_NUM_THREADS=1

NODES=($(scontrol show hostnames))

stream_node=${NODES[0]}
observer_node=${NODES[1]}

rm baseline_dgemm.csv

for mains in {1..104}
do
    echo running with "$mains"
    srun --ntasks=208 ./bin/ompi/MAIN_DGEMM_OP_FOP "$mains" 0 >> baseline_dgemm.csv
done

rm baseline_stream.csv

mains=1
for run in {0..10}
do
    echo running with "$mains"
    srun --ntasks=208 ./bin/ompi/MAIN_STREAM_OP_FOP "$mains" 0 >> baseline_stream.csv
done
