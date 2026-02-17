#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int my_rank, uni_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);
    if (argc != 2) {
        if (my_rank == 0) printf("Usage: %s <total_array_size>\n", argv[0]);
        MPI_Finalize()
        return 1;
    }

    int total_elements = atoi(argv[1]);
            if (total_elements % uni_size != 0) {
        if (my_rank == 0) {
            printf("Error: Array size (%d) must be evenly divisible by processes (%d).\n", 
                   total_elements, uni_size);
        }
        MPI_Finalize();
        return 1;
    }

    int chunk_size = total_elements / uni_size;
    int *full_array = NULL;
    int *local_chunk = (int *)malloc(chunk_size * sizeof(int));

    // Initialise
    if (my_rank == 0) {
        full_array = (int *)malloc(total_elements * sizeof(int));
        for (int i = 0; i < total_elements; i++) {
            full_array[i] = 1;
        }
        printf("[Master] Created array of %d elements. Distributing...\n", total_elements);
    }


    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    //  DISTRIBUTE
    MPI_Scatter(
        full_array, chunk_size, MPI_INT,
        local_chunk, chunk_size, MPI_INT,
        0, MPI_COMM_WORLD
    );

    //  COMPUTING
    long long local_sum = 0;
    for (int i = 0; i < chunk_size; i++) {
        local_sum += local_chunk[i];
    }

    //  COLLECTION
    long long global_sum = 0;
    MPI_Reduce(
        &local_sum, &global_sum, 1, MPI_LONG_LONG,
        MPI_SUM,0, MPI_COMM_WORLD
    );
    double end_time = MPI_Wtime();

    // Output
    if (my_rank == 0) {
        printf("--- Benchmarking Results ---\n");
        printf("Calculated Sum: %lld (Expected: %d)\n", global_sum, total_elements);
        printf("Total Time (Scatter + Compute + Reduce): %f seconds\n", end_time - start_time);
        free(full_array);
    }

     free(local_chunk);
    MPI_Finalize();
    return 0;
}
