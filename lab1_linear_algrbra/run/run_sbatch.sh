#!/bin/bash

echo "Runing $1"
sbatch $1
squeue

