#!/bin/bash
# argparse
if [ -n "$1" ]; then
    src_file=$1
else
    src_file="main.c"
fi
if [ -n "$2" ]; then
    out_file=$2
else
    out_file="main.out"
fi
if [ -n "$3" ]; then
    max_time=$3
else
    max_time="00:05:00"
fi
if [ -n "$4" ]; then
    sleep_time=$4
else
    sleep_time=0
fi
# Настройка констант
path=$(pwd)
params=(1 2 4 8 16)
counts=(1 1 1 1 2)
tmp=$path/tmp
exe_path=$tmp/exe
res_path=$tmp/res
# Создаем директории
mkdir $tmp
mkdir $exe_path
mkdir $res_path
# Загружаем MPI
module load mpi/mpich-x86_64
# Собираем наш файл
eval "build.sh ${src_file} mpecc ${out_file}"

echo "Successful build"

for ((i=0; i<${#params[@]}; i++)); do
    folder_name="${res_path}/${params[i]}" # Директория для скрипта и результата
    mkdir $folder_name
    name=("${folder_name}/run_${params[i]}_proc.sbatch")
    names+=($name)
    eval "gen_runing_sbatch.sh ${name} ${counts[i]} ${params[i]} "J${counts[i]}_${params[i]}_${src_file}" ${max_time} ${path}/${out_file}${params[i]}"
done

echo "Finish configuration"

# run all sbatch
for ((i=0; i<${#names[@]}; i++)); do
	echo ${names[i]}
    eval "run_sbatch.sh ${names[i]}"
    sleep $sleep_time
done

echo "Finish successful"

