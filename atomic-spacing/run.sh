#!/bin/bash
#SBATCH --ntasks=256
#SBATCH --ntasks-per-node=128
#SBATCH --time=03:00:00
#SBATCH --nodes=2
#SBATCH --cpu-freq=high
#SBATCH --exclusive
#SBATCH --constraint=no_monitoring


module purge
module load release/2026 GCC/14.3.0  OpenMPI/5.0.8

NODES=($(scontrol show hostnames))

source_node=${NODES[0]}
# observer_node1=${NODES[1]}

ntimes=2

rm output.csv

echo observers,offset,time >> output.csv


for observers in {58..62}
do
    file=bin/main_ompi
    echo running $file with $observers
    srun --nodes=1 --ntasks=1 -w "$source_node" ./"$file" "$ntimes" "$observers" : \
    --nodes=1 --ntasks="$observers" -w "${NODES[1]}" ./"$file" "$ntimes" "$observers"  >> output.csv
done

# ml purge

# module load intel-compilers/2025.2.0
# module load impi/2021.16.1

# for observers in {2..16}
# do
#     file=bin/main_impi
#     echo running $file with $observers
#     srun --nodes=1 --ntasks=1 -w "$source_node" ./"$file" : --nodes=1 --ntasks="$observers" -w "$observer_node" ./"$file" 
# done