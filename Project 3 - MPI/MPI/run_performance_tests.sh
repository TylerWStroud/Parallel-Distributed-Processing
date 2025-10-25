#!/bin/bash
echo "=== MPI Pi Calculation Peformance Analysis ==="
echo "Compiling MPI program.."
mpicc -g mpi-pi.c -o mpi-pi.o

echo ""
echo "Performance Results:"
echo "=================="

#tests different number of processes
for processes in 1 2 4; do
	echo ""
	echo "Testing with $processes processes:"
	mpirun -n $processes ./mpi-pi.o
done

echo ""
echo "=== Performance Comparison Complete === "
