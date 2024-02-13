#!/bin/bash

# Compilar matrixMultiply_eficiencia.cpp
g++ matrixMultiply_eficiencia.cpp -o matrix_eficiencia

# Compilar fork_matrixMultiply_eficiencia.cpp
g++ fork_matrixMultiply_eficiencia.cpp -o fork_eficiencia

# Compilar threads_matrixMultiply_eficiencia.cpp
g++ threads_matrixMultiply_eficiencia.cpp -o threads_eficiencia

# Compilar Openmp_matrixMultiply_eficiencia.cpp con soporte para OpenMP
g++ Openmp_matrixMultiply_eficiencia.cpp -o mmomp -fopenmp
