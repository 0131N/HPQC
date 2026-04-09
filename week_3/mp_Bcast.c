// Method 2: Broadcast (Everyone gets a full copy)
// NOTE: For this to work, every process must allocate memory for 'full_array', not just Rank 0.

MPI_Bcast(
    full_array,      // The array being copied
    total_elements,  // Total size (everyone gets everything)
    MPI_INT,         // Data type
    0,               // Rank doing the broadcasting
    MPI_COMM_WORLD
);
