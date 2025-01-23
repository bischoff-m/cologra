#!/usr/bin/zsh
########## Begin Slurm header ##########
#
# Assign job name
#SBATCH --job-name=cologra
#
# Request number of nodes for job
#SBATCH --nodes=32
#
# Number of program instances to be executed
#SBATCH --ntasks=32
#
# Maximum run time of job
#SBATCH --time=24:00:00
#
# Write standard output and errors in same file
#SBATCH --output=output.%J.txt
#
########### End Slurm header ##########

module load GCC
module load OpenMPI
module list

$MPIEXEC $FLAGS_MPI_BATCH build/src/cologra_cli
