int local_sum = 0; // The sum calculated by this specific process
int global_sum = 0; // Only the Master will have the correct value here at the end

// Calculate local_sum from your local_chunk...

// ALL processes call this line:
MPI_Reduce(
    &local_sum, &global_sum, 1, MPI_INT, 
    MPI_SUM,          // The math operation to perform!
    0, MPI_COMM_WORLD // The rank that gets the final answer
);

if (my_rank == 0) {
    printf("Final Sum is: %d\n", global_sum);
}
