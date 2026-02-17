#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void my_custom_sum(void *invec, void *inoutvec, int *len, MPI_Datatype *datatype) {
    long long *in = (long long *)invec;
    long long *inout = (long long *)inoutvec;

    for (int i = 0; i < *len; i++) {
        inout[i] = inout[i] + in[i]; 
    }
}

int main(int argc, char **argv) {
    int my_rank, uni_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    if (argc != 2) {
        if (my_rank == 0) printf("Usage: %s <total_array_size>\n", argv[0]);
        MPI_Finalize(); return 1;
    }

    int total_elements = atoi(argv[1]);
    int chunk_size = total_elements / uni_size;

    int *full_array = NULL;
    int *local_chunk = (int *)malloc(chunk_size * sizeof(int));

    if (my_rank == 0) {
        full_array = (int *)malloc(total_elements * sizeof(int));
        for (int i = 0; i < total_elements; i++) full_array[i] = 1;
    }

    MPI_Scatter(full_array, chunk_size, MPI_INT, local_chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    long long local_sum = 0;
    for (int i = 0; i < chunk_size; i++) {
        local_sum += local_chunk[i];
    }

    long long global_sum_builtin = 0;
    MPI_Barrier(MPI_COMM_WORLD);
    double start_builtin = MPI_Wtime();

    MPI_Reduce(&local_sum, &global_sum_builtin, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_builtin = MPI_Wtime();

    long long global_sum_custom = 0;
    MPI_Op MY_MPI_CUSTOM_SUM;

    MPI_Op_create(&my_custom_sum, 1, &MY_MPI_CUSTOM_SUM);

    MPI_Barrier(MPI_COMM_WORLD);
    double start_custom = MPI_Wtime();

    MPI_Reduce(&local_sum, &global_sum_custom, 1, MPI_LONG_LONG, MY_MPI_CUSTOM_SUM, 0, MPI_COMM_WORLD);

    double end_custom = MPI_Wtime();

    MPI_Op_free(&MY_MPI_CUSTOM_SUM);

    if (my_rank == 0) {
        printf("ACCURACY");
        printf("Built-in Sum: %lld\n", global_sum_builtin);
        printf("Custom Sum:   %lld\n", global_sum_custom);
        printf("PERFORMANCE");
        printf("Built-in Time: %f seconds\n", end_builtin - start_builtin);
        printf("Custom Time:   %f seconds\n\n", end_custom - start_custom);
        free(full_array);
    }

    free(local_chunk);
    MPI_Finalize();
    return 0;
}
