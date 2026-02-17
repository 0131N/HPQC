#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int my_rank, uni_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    if (uni_size != 2 || argc != 3) {
        if (my_rank == 0) printf("Usage: %s <num_pings> <bytes>\n", argv[0]);
        MPI_Finalize(); return 1;
    }

    int num_pings = atoi(argv[1]);
    int bytes = atoi(argv[2]);

    // integers per bytes
    int num_ints = bytes / sizeof(int);
    // Allocate array
    int *buffer = (int *)malloc(bytes);
    if (buffer == NULL) {
        printf("Memory allocation failed!\n");
        MPI_Finalize(); return 1;
    }
    // Initialize counter
    buffer[0] = 0;
    double start_time, end_time;

    if (my_rank == 0) {
        start_time = MPI_Wtime();
        while (buffer[0] < num_pings) {
            MPI_Send(buffer, num_ints, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer, num_ints, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end_time = MPI_Wtime();
        double elapsed_time = end_time - start_time;
        double time_per_message = elapsed_time / (2.0 * num_pings);
        printf("Size: %d Bytes | Avg Time per msg: %f s\n", bytes, time_per_message);
    } else if (my_rank == 1) {
        while (buffer[0] < num_pings) {
            MPI_Recv(buffer, num_ints, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            buffer[0]++; // Increment counter
            MPI_Send(buffer, num_ints, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    free(buffer);
    MPI_Finalize();
    return 0;
}
