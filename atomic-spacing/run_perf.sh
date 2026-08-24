#!/bin/bash
#SBATCH --ntasks=208
#SBATCH --ntasks-per-node=104
#SBATCH --time=01:00:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=high
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring


module purge
module load release/2026 GCC/14.3.0  OpenMPI/5.0.8

NODES=($(scontrol show hostnames))

source_node=${NODES[0]}
# observer_node1=${NODES[1]}

ntimes=5

rm output_perf.csv
rm perf_results/*

echo observers,offset,time >> output_perf.csv

observers=58

export UCX_LOG_LEVEL=info

file=bin/main_ompi
echo running $file with $observers
srun \
  --nodes=1 --ntasks=1 -w "$source_node" \
  perf record -o perf_results/perf_target58.data ./"$file" "$ntimes" "$observers" : \
  --nodes=1 --ntasks="$observers" -w "${NODES[1]}" \
  bash -c 'perf record -o "perf_results/${SLURM_PROCID}_58.data" ./"$1" "$2" "$3"' _ \
  "$file" "$ntimes" "$observers"


observers=60

echo "script sees: <$SLURM_PROCID>"
set -x

export UCX_LOG_LEVEL=info

file=bin/main_ompi
echo running $file with $observers
srun \
  --nodes=1 --ntasks=1 -w "$source_node" \
  perf record -o perf_results/perf_target60.data ./"$file" "$ntimes" "$observers" : \
  --nodes=1 --ntasks="$observers" -w "${NODES[1]}" \
  bash -c 'perf record -o "perf_results/${SLURM_PROCID}_60.data" ./"$1" "$2" "$3"' _ \
  "$file" "$ntimes" "$observers"

# srun \
#   --nodes=1 --ntasks=1 -w "$source_node" \
#   bash -c 'echo FIRST: PROCID=$SLURM_PROCID' : \
#   --nodes=1 --ntasks="$observers" -w "${NODES[1]}" \
#   bash -c 'echo SECOND: PROCID=$SLURM_PROCID LOCALID=$SLURM_LOCALID HOST=$(hostname)'

# ml purge

# module load intel-compilers/2025.2.0
# module load impi/2021.16.1

# for observers in {2..16}
# do
#     file=bin/main_impi
#     echo running $file with $observers
#     srun --nodes=1 --ntasks=1 -w "$source_node" ./"$file" : --nodes=1 --ntasks="$observers" -w "$observer_node" ./"$file" 
# done