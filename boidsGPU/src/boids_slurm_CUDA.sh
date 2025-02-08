#!/bin/bash -l

#SBATCH --partition=cosc3500 
#SBATCH --gres=gpu:1
#SBATCH --account=cosc3500
#SBATCH --job-name=boids
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --time=0-00:10:00

module load compiler-rt/latest
module load mkl/latest
module load mpi/openmpi-x86_64
module load cuda/11.1

export PATH=/opt/local/stow/cuda-11.1/bin:$PATH
export PATH=/usr/lib64/openmpi/bin:$PATH

make clean

nvcc -std=c++11 -O3 -Xcompiler -fopenmp -o simulator simulator.cpp flock.cpp boid.cpp grid.cpp cuda_flock.cu

hostname
time ./simulator
#nsys stats report1.qdrep