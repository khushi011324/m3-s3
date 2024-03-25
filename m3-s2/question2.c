#include <mpi.h>
#include <stdio.h>

int main(int argc, char * argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Master process
        char message[] = "Hello, World!";
        // Send the message to worker processes
        for (int i = 1; i < size; i++) {
            MPI_Send(message, sizeof(message), MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Worker processes
        char recv_message[100]; // Assuming max message length is 100 characters
        // Receive the message from the master process
        MPI_Recv(recv_message, sizeof(recv_message), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received message: %s\n", rank, recv_message);
    }

    MPI_Finalize();
    return 0;
}
