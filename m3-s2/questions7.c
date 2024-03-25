#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

using namespace std::chrono;

void randomVector(int vector[], int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 100;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    unsigned long global_size = 100000000;
    unsigned long per_process_size = global_size / size;
    int *v1 = (int *)malloc(per_process_size * sizeof(int));
    int *v2 = (int *)malloc(per_process_size * sizeof(int));
    int *v3 = (int *)malloc(per_process_size * sizeof(int));

    srand(time(NULL) + rank); 
    randomVector(v1, per_process_size);
    randomVector(v2, per_process_size);

    auto start = high_resolution_clock::now();

    int *recieve_v1 = (int *)malloc(per_process_size * sizeof(int));
    int *recieve_v2 = (int *)malloc(per_process_size * sizeof(int));

    MPI_Scatter(v1, per_process_size, MPI_INT, recieve_v1, per_process_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(v2, per_process_size, MPI_INT, recieve_v2, per_process_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < per_process_size; i++) {
        v3[i] = recieve_v1[i] + recieve_v2[i];
    }

     int total_sum = 0;

     int sum_v3 = 0;
     for (int i = 0; i < per_process_size; i++) {
        sum_v3 += v3[i];
     }

     MPI_Reduce(&sum_v3, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    if (rank == 0) {
        std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;
       std::cout << "Total sum of v3: " << total_sum << std::endl;
    }

    
    MPI_Finalize();
    return 0;
}
