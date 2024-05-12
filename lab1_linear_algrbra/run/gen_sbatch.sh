#!/bin/bash

file_name=$1
count_hosts=$2
count_procs=$3
job_name=$4
my_time=$5
exe_file=$6


touch $file_name
echo "#!/bin/bash" > $file_name
echo "#SBATCH -J job_name" >> $file_name
echo "#SBATCH -p compclass" >> $file_name
echo "#SBATCH -o ${exe_file}.%j.output" >> $file_name
echo "#SBATCH -e ${exe_file}.%j.output" >> $file_name
echo "#SBATCH -N $count_hosts" >> $file_name
echo "#SBATCH -n $count_procs" >> $file_name
echo "#SBATCH -t $my_time" >> $file_name

echo "module load nvidia/cuda" >> $file_name

echo "module load mpi/mpich-x86_64" >> $file_name

echo "export MPIEXEC_PORT_RANGE=23000:23100" >> $file_name
echo "mpiexec -n $count_procs $exe_file" >> $file_name


