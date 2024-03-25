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

  int *local_v1, *local_v2, *local_v3;

  local_v1 = (int *)malloc(local_size * sizeof(int));
  local_v2 = (int *)malloc(local_size * sizeof(int));
  local_v3 = (int *)malloc(local_size * sizeof(int));

  if (rank == 0) {
    // Master process generates random vectors for v1 and v2
    int *v1 = (int *)malloc(total_size * sizeof(int));
    int *v2 = (int *)malloc(total_size * sizeof(int));
    randomVector(v1, total_size);
    randomVector(v2, total_size);

    // Broadcast the data to all processes
    MPI_Bcast(v1, total_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(v2, total_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Free memory allocated for v1 and v2
    free(v1);
    free(v2);
  } else {
    // Worker processes receive the broadcasted data
    MPI_Bcast(local_v1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(local_v2, local_size, MPI_INT, 0, MPI_COMM_WORLD);
  }

  // Perform local computation
  auto start = high_resolution_clock::now();
  for (unsigned long i = 0; i < local_size; i++) {
    local_v3[i] = local_v1[i] + local_v2[i];
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);

  // Gather the results back to the master process
  int *v3 = NULL;
  if (rank == 0) {
    v3 = (int *)malloc(total_size * sizeof(int));
  }
  MPI_Gather(local_v3, local_size, MPI_INT, v3, local_size, MPI_INT, 0, MPI_COMM_WORLD);

  // Print timing information from the master process
  if (rank == 0) {
    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
    // Free memory allocated for v3
    free(v3);
  }

  // Free memory allocated for local vectors
  free(local_v1);
  free(local_v2);
  free(local_v3);

  MPI_Finalize();
  return 0;
}
