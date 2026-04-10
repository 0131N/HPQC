void worker_code(int my_rank) {
    int send_message = my_rank * 10;
    int dest = 0;
    
    // 1. Get Start Time
    double start_time = MPI_Wtime();

    // 2. Perform Communication
    // You can swap MPI_Send for MPI_Ssend here to compare times
    MPI_Send(&send_message, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

    // 3. Get End Time
    double end_time = MPI_Wtime();

    printf("[Rank %d] Communication time: %f seconds\n", my_rank, end_time - start_time);
}
