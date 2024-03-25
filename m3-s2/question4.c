#include <chrono>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <mpi.h>

using namespace std::chrono;
using namespace std;

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

  unsigned long total_size = 100000000;
  unsigned long local_size = total_size / size;

  srand(time(0) + rank); // Ensure different random numbers for each process

  int *local_v1 = new int[local_size];
  int *local_v2 = new int[local_size];
  int *local_v3 = new int[local_size];

  // Scatter the data among processes
  MPI_Scatter(NULL, local_size, MPI_INT, local_v1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(NULL, local_size, MPI_INT, local_v2, local_size, MPI_INT, 0, MPI_COMM_WORLD);

  // Perform local computation
  auto start = high_resolution_clock::now();
  for (unsigned long i = 0; i < local_size; i++) {
    local_v3[i] = local_v1[i] + local_v2[i];
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);

  // Reduce local sums to obtain the global sum
  int global_sum;
  MPI_Reduce(&local_v3[0], &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  // Print timing information and global sum from the master process
  if (rank == 0) {
    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
    cout << "Total sum of elements in v3: " << global_sum << endl;
  }

  // Free memory allocated for local vectors
  delete[] local_v1;
  delete[] local_v2;
  delete[] local_v3;

  MPI_Finalize();
  return 0;
}
