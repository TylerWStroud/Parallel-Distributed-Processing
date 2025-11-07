#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>

#define N 2048
#define FactorIntToDouble 1.1

double firstMatrix [N] [N] = {0.0};
double secondMatrix [N] [N] = {0.0};
double matrixMultiResult [N] [N] = {0.0};

void matrixMulti()
{
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for(int row = 0 ; row < N ; row++){
        for(int col = 0; col < N ; col++){
            double resultValue = 0;
            for(int transNumber = 0 ; transNumber < N ; transNumber++) {
                resultValue += firstMatrix [row] [transNumber] * secondMatrix [transNumber] [col];
            }
            matrixMultiResult [row] [col] = resultValue;
        }
    }
}

// Alternative optimized version with better cache performance
void matrixMulti_optimized()
{
    #pragma omp parallel for schedule(dynamic)
    for(int row = 0 ; row < N ; row++){
        for(int col = 0; col < N ; col++){
            double resultValue = 0;
            // Inner loop with contiguous memory access
            for(int transNumber = 0 ; transNumber < N ; transNumber++) {
                resultValue += firstMatrix [row] [transNumber] * secondMatrix [transNumber] [col];
            }
            matrixMultiResult [row] [col] = resultValue;
        }
    }
}

// Even more optimized version with loop reordering
void matrixMulti_cache_optimized()
{
    #pragma omp parallel for schedule(dynamic)
    for(int row = 0 ; row < N ; row++){
        for(int transNumber = 0 ; transNumber < N ; transNumber++){
            double temp = firstMatrix [row] [transNumber];
            for(int col = 0; col < N ; col++) {
                matrixMultiResult [row] [col] += temp * secondMatrix [transNumber] [col];
            }
        }
    }
}

void matrixInit()
{
    #pragma omp parallel for collapse(2)
    for(int row = 0 ; row < N ; row++ ) {
        for(int col = 0 ; col < N ;col++){
            // Each thread needs its own seed to avoid race conditions
            unsigned int seed = row * N + col;
            firstMatrix [row] [col] = ( rand_r(&seed) % 10 ) * FactorIntToDouble;
            secondMatrix [row] [col] = ( rand_r(&seed) % 10 ) * FactorIntToDouble;
        }
    }
}

int main()
{
    printf("Matrix size: %d x %d\n", N, N);
    printf("Number of threads available: %d\n", omp_get_max_threads());
    
    // Initialize matrices
    double init_start = omp_get_wtime();
    matrixInit();
    double init_end = omp_get_wtime();
    printf("Initialization time: %.3f seconds\n", init_end - init_start);

    // Parallel matrix multiplication
    double t1 = omp_get_wtime();
    matrixMulti();  // Using the basic parallel version
    double t2 = omp_get_wtime();
    
    printf("Parallel multiplication time: %.3f seconds\n", t2 - t1);

    return 0;
}