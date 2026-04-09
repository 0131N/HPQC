// ... Inside the time step loop ...

// 1. GHOST CELL EXCHANGE
double left_ghost = 0.0;
double right_edge = local_positions[local_points - 1];

// Send right edge to Rank + 1, receive left ghost from Rank - 1
if (rank > 0) {
    // Receive from left neighbor
    MPI_Recv(&left_ghost, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
if (rank < size - 1) {
    // Send to right neighbor
    MPI_Send(&right_edge, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
}

// 2. UPDATE POSITIONS
// Create temporary array for new local positions
double* new_local = (double*) malloc(local_points * sizeof(double));

if (rank == 0) {
    new_local[0] = driver(current_time); // Rank 0 is driven by the oscillator
} else {
    new_local[0] = left_ghost; // Other ranks follow their left neighbor's ghost cell
}

// Update the rest of the local points
for (int j = 1; j < local_points; j++) {
    new_local[j] = local_positions[j - 1];
}

// Copy back to main local array
for (int j = 0; j < local_points; j++) {
    local_positions[j] = new_local[j];
}
free(new_local);

// 3. AGGREGATION & WRITING
// Gather all local arrays back into the 'global_positions' array on Rank 0
MPI_Gather(local_positions, local_points, MPI_DOUBLE, 
           global_positions, local_points, MPI_DOUBLE, 
           0, MPI_COMM_WORLD);

// Rank 0 writes the global_positions to the CSV file
if (rank == 0) {
    // ... fprintf to CSV ...
}
