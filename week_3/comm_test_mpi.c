// comm_test_mpi_v3.c
#include <stdio.h>
#include <mpi.h>

void run_master(int total_processes) {
    int recv_message;
    MPI_Status status;
    for (int i = 1; i < total_processes; i++) {
        MPI_Recv(&recv_message, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        printf("[Master] Received %d from Rank %d\n", recv_message, i);
    }
}

// NEW FUNCTION: Handles everything other ranks need to do
void run_worker(int rank) {
    int send_message = rank * 10;
    MPI_Send(&send_message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    printf("[Worker] Rank %d sent %d\n", rank, send_message);
}

int main(int argc, char **argv) {
    int my_rank, uni_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    if (uni_size > 1) {
        if (my_rank == 0) {
            run_master(uni_size);
        } 
        else {
            run_worker(my_rank); // CALL THE NEW FUNCTION
        }
    } 
    else {
        printf("Error: Need at least 2 processes.\n");
    }

    MPI_Finalize();
    return 0;
}
