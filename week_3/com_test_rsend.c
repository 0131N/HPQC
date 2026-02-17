// comm_test_mpi_final.c
#include <stdio.h>
#include <mpi.h>

void run_master(int total_processes) {
    int recv_message;
    MPI_Status status;
    
    printf("--- Master Starting Reception ---\n");
    for (int i = 1; i < total_processes; i++) {
        MPI_Recv(&recv_message, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        printf("[Master] Received %d from Rank %d\n", recv_message, i);
    }
}

void worker_code_rsend(int my_rank) {
    int send_message = my_rank * 10;
    int dest = 0;
    int tag = 0;

    // OPTIONAL: A barrier ensures everyone enters this section together,
    // increasing the chance that Rank 0 is ready.
    MPI_Barrier(MPI_COMM_WORLD); 

    printf("[Worker %d] Rsend (Ready)...\n", my_rank);
    
    // Ready Send: Only works if the matching Receive is already posted!
    MPI_Rsend(&send_message, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);

    printf("[Worker %d] Rsend complete.\n", my_rank);
}

int main(int argc, char **argv) {
    int my_rank, uni_size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    // GUARD CLAUSE: Check for errors first, then proceed
    if (uni_size < 2) {
        if (my_rank == 0) printf("Error: Run with at least 2 processes (e.g. -np 2)\n");
        MPI_Finalize();
        return 1;
    }

    // Clean logic flow
    if (my_rank == 0) {
        run_master(uni_size);
    } else {
        run_worker(my_rank);
    }

    MPI_Finalize();
    return 0;
}
