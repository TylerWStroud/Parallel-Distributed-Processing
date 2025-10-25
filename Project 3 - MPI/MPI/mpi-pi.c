#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define NUMSTEPS 1000000

int main(int argc, char *argv[]) {
    int i;
    int rank, size;
    double local_sum = 0.0, global_sum = 0.0;
    double x, pi;
    double step;
    struct timespec start, end;
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Only rank 0 records the start time
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start);
    }
    
    step = 1.0 / (double)NUMSTEPS;
    
    // Calculate the portion of work for this process
    int local_steps = NUMSTEPS / size;
    int start_index = rank * local_steps;
    int end_index = (rank == size - 1) ? NUMSTEPS : start_index + local_steps;
    
    // Each process calculates its portion of the sum
    for (i = start_index; i < end_index; i++) {
        x = (i + 0.5) * step;  // midpoint of the interval
        local_sum += 4.0 / (1.0 + x * x);
    }
    
    // Reduce all local sums to get the global sum at rank 0
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    // Rank 0 calculates the final result and prints it
    if (rank == 0) {
        pi = step * global_sum;
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        u_int64_t diff = 1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
        
        printf("PI is %.20f\n", pi);
        printf("elapsed time = %llu nanoseconds\n", (long long unsigned int)diff);
    }
    
    MPI_Finalize();
    return 0;
}