#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int my_rank, uni_size, ierror;
    
    ierror = MPI_Init(&argc, &argv);
    ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    //  Error Checking
    if (uni_size != 2) {
        if (my_rank == 0) printf("Error: Ping-pong requires exactly 2 processes.\n");
        MPI_Finalize();
        return 1;
    }
    if (argc != 2) {
        if (my_rank == 0) printf("Usage: %s <num_pings>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int num_pings = atoi(argv[1]);
    int counter = 0;
    double start_time, end_time, elapsed_time, time_per_message;


    if (my_rank == 0) { // ROOT PROCESS
        start_time = MPI_Wtime();

        while (counter < num_pings) {
            MPI_Send(&counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        end_time = MPI_Wtime();
        elapsed_time = end_time - start_time;

        // time per message
        time_per_message = elapsed_time / (2.0 * num_pings);

        printf("Pings: %d | Total Time: %f s | Avg Latency: %f s\n", 
               num_pings, elapsed_time, time_per_message);

    } else if (my_rank == 1) { // CLIENT PROCESS
        while (counter < num_pings) {
            // Receive ping from Root
            MPI_Recv(&counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                      counter++;
                      MPI_Send(&counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
