#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int my_rank, uni_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    if (argc != 2) {
        if (my_rank == 0) printf("Usage: %s <total_elements>\n", argv[0]);
        MPI_Finalize();
        return -1;
    }

    // Parse array size and calculate chunks
    long long N = atoll(argv[1]);
    long long chunk_size = N / uni_size;
    
    // Memory Allocation
    int *full_array = NULL;
    int *local_chunk = (int *)malloc(chunk_size * sizeof(int));
    int *bcast_array = (int *)malloc(N * sizeof(int)); // Workers need full memory for Bcast
    long long *gather_sums = NULL;

    if (my_rank == 0) {
        full_array = (int *)malloc(N * sizeof(int));
        gather_sums = (long long *)malloc(uni_size * sizeof(long long));
        // Initialize the array with 1s
        for (long long i = 0; i < N; i++) {
            full_array[i] = 1; 
            bcast_array[i] = 1;
        }
    }

    double start_time, end_time;
    long long local_sum, global_sum;

    /* ======================================================================
       METHOD 1: The "DIY" Method (Manual Send/Recv)
       ====================================================================== */
    MPI_Barrier(MPI_COMM_WORLD); // Sync everyone before starting the timer
    start_time = MPI_Wtime();
    local_sum = 0;
    global_sum = 0;

    // Distribute
    if (my_rank == 0) {
        for (int i = 1; i < uni_size; i++) {
            MPI_Send(&full_array[i * chunk_size], chunk_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        for (long long i = 0; i < chunk_size; i++) local_chunk[i] = full_array[i];
    } else {
        MPI_Recv(local_chunk, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Compute
    for (long long i = 0; i < chunk_size; i++) local_sum += local_chunk[i];

    // Collect
    if (my_rank == 0) {
        global_sum = local_sum;
        long long temp_rec;
        for (int i = 1; i < uni_size; i++) {
            MPI_Recv(&temp_rec, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_sum += temp_rec;
        }
    } else {
        MPI_Send(&local_sum, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    end_time = MPI_Wtime();
    if (my_rank == 0) printf("Method 1 (DIY Send/Recv): Sum = %lld, Time = %f seconds\n", global_sum, end_time - start_time);


    /* ======================================================================
       METHOD 2: Broadcast & Gather
       ====================================================================== */
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();
    local_sum = 0;
    global_sum = 0;

    // Distribute: Everyone gets a copy of the WHOLE array
    MPI_Bcast(bcast_array, N, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute: Workers must calculate their specific offset to only sum their portion
    long long start_index = my_rank * chunk_size;
    for (long long i = start_index; i < start_index + chunk_size; i++) {
        local_sum += bcast_array[i];
    }

    // Collect: Gather all local sums into an array on the Root
    MPI_Gather(&local_sum, 1, MPI_LONG_LONG, gather_sums, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Root manually adds the gathered array
    if (my_rank == 0) {
        for (int i = 0; i < uni_size; i++) global_sum += gather_sums[i];
    }

    end_time = MPI_Wtime();
    if (my_rank == 0) printf("Method 2 (Bcast & Gather): Sum = %lld, Time = %f seconds\n", global_sum, end_time - start_time);


    /* ======================================================================
       METHOD 3: Scatter & Reduce (The Gold Standard)
       ====================================================================== */
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();
    local_sum = 0;
    global_sum = 0;

    // Distribute: MPI slices the array and deals the chunks automatically
    MPI_Scatter(full_array, chunk_size, MPI_INT, local_chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute
    for (long long i = 0; i < chunk_size; i++) local_sum += local_chunk[i];

    // Collect: MPI collects and sums in one step
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();
    if (my_rank == 0) printf("Method 3 (Scatter & Reduce): Sum = %lld, Time = %f seconds\n", global_sum, end_time - start_time);


    // Cleanup memory
    if (my_rank == 0) {
        free(full_array);
        free(gather_sums);
    }
    free(local_chunk);
    free(bcast_array);
    
    MPI_Finalize();
    return 0;
}
