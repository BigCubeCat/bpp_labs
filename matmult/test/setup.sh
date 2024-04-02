#!/bin/bash

mkdir ~/22209/bit/eff
mkdir ~/22209/bit/eff/err
mkdir ~/22209/bit/eff/out
mkdir ~/22209/bit/eff/output
cp ~/22209/bit/matmult ~/22209/bit/eff/matmult1
cp ~/22209/bit/matmult ~/22209/bit/eff/matmult2
cp ~/22209/bit/matmult ~/22209/bit/eff/matmult4
cp ~/22209/bit/matmult ~/22209/bit/eff/matmult8
cp ~/22209/bit/matmult ~/22209/bit/eff/matmult16

cp ~/22209/bit/input.mat ~/22209/bit/eff/input.mat

mkdir ~/22209/bit/grid
mkdir ~/22209/bit/grid/err
mkdir ~/22209/bit/grid/out
mkdir ~/22209/bit/grid/output
cp ~/22209/bit/matmult ~/22209/bit/grid/matmult1x16
cp ~/22209/bit/matmult ~/22209/bit/grid/matmult2x8
cp ~/22209/bit/matmult ~/22209/bit/grid/matmult4x4
cp ~/22209/bit/matmult ~/22209/bit/grid/matmult8x2
cp ~/22209/bit/matmult ~/22209/bit/grid/matmult16x1

mkdir ~/22209/bit/count
mkdir ~/22209/bit/count/data
mkdir ~/22209/bit/count/err
mkdir ~/22209/bit/count/out
mkdir ~/22209/bit/count/output
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_1K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_2K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_3K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_4K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_5K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_6K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_7K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_8K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_9K
cp ~/22209/bit/matmult ~/22209/bit/count/matmult_10K

./input_generator.py ~/22209/bit/count/data/1k.mat 1000 1000 1000
./input_generator.py ~/22209/bit/count/data/2k.mat 2000 2000 2000
./input_generator.py ~/22209/bit/count/data/3k.mat 3000 3000 3000
./input_generator.py ~/22209/bit/count/data/4k.mat 4000 4000 4000
./input_generator.py ~/22209/bit/count/data/5k.mat 5000 5000 5000
./input_generator.py ~/22209/bit/count/data/6k.mat 6000 6000 6000
./input_generator.py ~/22209/bit/count/data/7k.mat 7000 7000 7000
./input_generator.py ~/22209/bit/count/data/8k.mat 8000 8000 8000
./input_generator.py ~/22209/bit/count/data/9k.mat 9000 9000 9000
./input_generator.py ~/22209/bit/count/data/10k.mat 10000 10000 10000
