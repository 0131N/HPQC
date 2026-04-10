// Assuming 'total_elements' is the full array size, and 'chunk_size' is total_elements / uni_size
int *full_array = NULL;
int *local_chunk = malloc(chunk_size * sizeof(int));

if (my_rank == 0) {
    full_array = malloc(total_elements * sizeof(int));
    // ... initialize full_array with 1s ...
}

// ALL processes call this line at the same time:
MPI_Scatter(
    full_array, chunk_size, MPI_INT,  // Send buffer (Root only cares about this)
    local_chunk, chunk_size, MPI_INT, // Receive buffer (Everyone puts data here)
    0, MPI_COMM_WORLD                 // Root rank
);
