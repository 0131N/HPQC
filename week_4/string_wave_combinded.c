#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

// Struct to hold our simulation parameters
typedef struct {
    int points;
    int cycles;
    int samples;
    char filepath[256];
} SimConfig;

// Function prototypes
SimConfig check_args(int argc, char **argv);
void initialise_vector(double vector[], int size, double initial);
void print_header(FILE** p_out_file, int points);
double driver(double time);

int main(int argc, char **argv)
{
    // 1. INITIALIZE MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 2. PARSE ARGUMENTS
    // All MPI ranks parse the command line arguments simultaneously
    SimConfig config = check_args(argc, argv);

    // Ensure the number of points divides evenly among processors for simplicity
    if (config.points % size != 0) {
        if (rank == 0) {
            printf("Error: Number of points (%d) must be evenly divisible by the number of MPI processes (%d).\n", config.points, size);
        }
        MPI_Finalize();
        return -1;
    }

    // Calculate time steps and local domain size
    int time_steps = config.cycles * config.samples + 1; 
    double step_size = 1.0 / config.samples;
    int local_points = config.points / size;

    // 3. ALLOCATE MEMORY
    // Every rank needs a local array for its chunk of the string
    double* local_positions = (double*) malloc(local_points * sizeof(double));
    double* new_local = (double*) malloc(local_points * sizeof(double));
    initialise_vector(local_positions, local_points, 0.0);

    // Only Rank 0 needs the global array for writing to the file
    double* global_positions = NULL;
    FILE* out_file = NULL;

    if (rank == 0) {
        global_positions = (double*) malloc(config.points * sizeof(double));
        out_file = fopen(config.filepath, "w");
        if (out_file == NULL) {
            printf("Error: Could not open file %s\n", config.filepath);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        print_header(&out_file, config.points);
    }
	MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();
    // 4. MAIN SIMULATION LOOP
    for (int i = 0; i < time_steps; i++)
    {
        double current_time = i * step_size;
        double left_ghost = 0.0;
        double right_edge = local_positions[local_points - 1];

        // --- GHOST CELL COMMUNICATION ---
        // Ranks receive from their left neighbor (except Rank 0)
        if (rank > 0) {
            MPI_Recv(&left_ghost, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        // Ranks send to their right neighbor (except the last Rank)
        if (rank < size - 1) {
            MPI_Send(&right_edge, 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }

        // --- UPDATE LOCAL POSITIONS ---
        // The first point in the local chunk depends on the rank
        if (rank == 0) {
            new_local[0] = driver(current_time); // Rank 0 is driven by the oscillator
        } else {
            new_local[0] = left_ghost;           // Other ranks follow the left ghost cell
        }

        // The rest of the points follow the leader within the local chunk
        for (int j = 1; j < local_points; j++) {
            new_local[j] = local_positions[j - 1];
        }

        // Copy updated positions back to the main local array
        for (int j = 0; j < local_points; j++) {
            local_positions[j] = new_local[j];
        }

        // --- AGGREGATION & FILE I/O ---
        // Gather all local_positions into global_positions on Rank 0
        MPI_Gather(local_positions, local_points, MPI_DOUBLE, 
                   global_positions, local_points, MPI_DOUBLE, 
                   0, MPI_COMM_WORLD);

        // Rank 0 writes the aggregated data to disk
        if (rank == 0) {
            fprintf(out_file, "%d, %lf", i, current_time);
            for (int j = 0; j < config.points; j++) {
                fprintf(out_file, ", %lf", global_positions[j]);
            }
            fprintf(out_file, "\n");
        }
    }
    double end_time = MPI_Wtime();
    double elapsed = end_time - start_time;
   

    if (rank == 0) {
       printf("\nSimulation Completed.\n");
       printf("Processors used: %d\n", size);
       printf("Total points:    %d\n", config.points);
       printf("Wall-clock time: %f seconds\n", elapsed);
       if (out_file) fclose(out_file);
       if (global_positions) free(global_positions);
       printf("MPI Simulation complete. Data written to %s\n", config.filepath);
 }

    free(local_positions);
    free(new_local);
    MPI_Finalize();
    return 0;
}

// Helper Functions
SimConfig check_args(int argc, char **argv) {
    SimConfig config;
    if (argc != 5) {
        // Only let Rank 0 print the error to avoid terminal spam
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank == 0) {
            printf("Usage: %s <points> <cycles> <samples> <output_filepath>\n", argv[0]);
        }
        MPI_Finalize();
        exit(-1);
    }

    config.points = atoi(argv[1]);
    config.cycles = atoi(argv[2]);
    config.samples = atoi(argv[3]);
    strncpy(config.filepath, argv[4], sizeof(config.filepath) - 1);
    config.filepath[sizeof(config.filepath) - 1] = '\0';

    return config;
}

void initialise_vector(double vector[], int size, double initial) {
    for (int i = 0; i < size; i++) {
        vector[i] = initial;
    }
}

void print_header(FILE** p_out_file, int points) {
    fprintf(*p_out_file, "#, time");
    for (int j = 0; j < points; j++) {
        fprintf(*p_out_file, ", y[%d]", j);
    }
    fprintf(*p_out_file, "\n");
}

double driver(double time) {
    return sin(time * 2.0 * M_PI);
}
