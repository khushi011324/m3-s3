#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Master process
        char message[] = "Hello, World!";
        for (int dest_rank = 1; dest_rank < size; dest_rank++) {
            MPI_Send(message, sizeof(message), MPI_CHAR, dest_rank, 0, MPI_COMM_WORLD);
        }
    } 

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes

    if (rank != 0) {
        // Worker processes
        char recv_message[100]; // Assuming max message length is 100 characters
        MPI_Recv(recv_message, sizeof(recv_message), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received message: %s\n", rank, recv_message);
    }

    MPI_Finalize();
    return 0;
}
