#!/bin/bash

module load mpi/intelmpi
module load itac

cmake CMakeLists.txt -B build
cd build
make
cd ..
