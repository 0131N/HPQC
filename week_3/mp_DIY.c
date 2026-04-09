// Method 1: Manual Send/Recv Loop
if (my_rank == 0) {
    // 1. Root sends a specific chunk to each worker
    for (int i = 1; i < uni_size; i++) {
        MPI_Send(&full_array[i * chunk_size], chunk_size, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    // 2. Root copies its own chunk locally
    for (int i = 0; i < chunk_size; i++) {
        local_chunk[i] = full_array[i];
    }
} else {
    // 3. Workers wait to receive their specific chunk
    MPI_Recv(local_chunk, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
