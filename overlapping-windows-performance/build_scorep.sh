module purge

mkdir bin
mkdir bin/scorep

scorep_path_gcc=/data/horse/ws/koka490e-mpi_rma/scorep-tooling-infiniband/scorep_install_gcc/bin/scorep
scorep_path_intel=/data/horse/ws/koka490e-mpi_rma/scorep-tooling-infiniband/scorep_install_intel/bin/scorep

source_path_gcc=/data/horse/ws/koka490e-mpi_rma/scorep-tooling-infiniband/source_env_foss
source_path_intel=/data/horse/ws/koka490e-mpi_rma/scorep-tooling-infiniband/source_env_intel

opt=-O2

(
    
    source "$source_path_gcc"
    "$scorep_path_gcc" --user mpicc "$opt" main.c -o bin/scorep/main_ompi

)

(
    
    source "$source_path_intel"
    "$scorep_path_intel" --user mpicc "$opt" main.c -o bin/scorep/main_impi

)

# source ../scorep-tooling-infiniband/source_env_intel

# "$scorep_path" --user mpiicx "$opt" main.c -o bin/scorep/main_impi