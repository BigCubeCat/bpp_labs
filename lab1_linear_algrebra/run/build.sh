#!/bin/bash

if [ -n "$1" ]; then
    src_file=$1
else
    src_file="main.c"
fi

if [ -n "$2" ]; then
    cmplr=$2
else
    cmplr="mpecc"
fi

if [ -n "$3" ]; then
    exe_file=$3
else
    exe_file="$src_file.out"
fi


echo "Compilling file ${src_file} (out is ${exe_file}) with ${cmplr}..."


echo "${cmplr} ${src_file} -O3 -o ${exe_file} -pthread -mpilog -Wall -Werror -Wextra"
eval "${cmplr} ${src_file} -O3 -o ${exe_file} -pthread -mpilog -Wall -Werror -Wextra"

cp $exe_file "${exe_file}2"
cp $exe_file "${exe_file}4"
cp $exe_file "${exe_file}8"
cp $exe_file "${exe_file}16"
mv $exe_file "${exe_file}1"

