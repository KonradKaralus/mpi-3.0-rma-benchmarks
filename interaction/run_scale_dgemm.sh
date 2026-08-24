#!/bin/bash
#SBATCH --ntasks=208
#SBATCH --ntasks-per-node=104
#SBATCH --time=01:00:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=2000000
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring
#SBATCH -c 1

ml purge

# module load release/2026 GCC/14.3.0  OpenMPI/5.0.8
module load release/2026 GCC/14.3.0  OpenMPI/5.0.8 OpenBLAS/0.3.30
export OMP_NUM_THREADS=1
export OPENBLAS_NUM_THREADS=1

NODES=($(scontrol show hostnames))

stream_node=${NODES[0]}
observer_node=${NODES[1]}

observers=32

rm out_scale_dgemm.csv

for dgemms in {1..104}
do
    # for file in bin/ompi/*
    # do
    file=bin/ompi/MAIN_DGEMM_OP_GET_STRIDE
        echo running $file with $dgemms dgemms
        # echo running stream with $file with $observers observers
        # srun --time=00:01:00 --nodes=1 --ntasks=1 -w "$stream_node" ./"$file" : --nodes=1 --ntasks="$observers" -w "$observer_node" ./"$file" 
        srun --ntasks=208 ./"$file" "$dgemms" "$observers" >> out_scale_dgemm.csv


        # echo running blas with $file with $observers observers
        # srun --nodes=1 --ntasks=1 -w "$stream_node" ./bin/ompi/blas : --nodes=1 --ntasks="$observers" -w "$observer_node" ./"$file" 
    # done
done