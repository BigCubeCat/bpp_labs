#!/bin/bash

module load mpi/intelmpi
module load itac

cmake CMakeLists.txt -B build
cd build
make
cp lab5 ../res/cluster
cd ..

