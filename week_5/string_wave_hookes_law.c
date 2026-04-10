#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

// Struct to hold simulation parameters
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

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 1. Parse Arguments
    SimConfig config = check_args(argc, argv);

    if (config.points % size != 0) {
        if (rank == 0) printf("Error: Points (%d) must be evenly divisible by processes (%d).\n", config.points, size);
        MPI_Finalize();
        return -1;
    }

    int time_steps = config.cycles * config.samples + 1;
    double dt = 1.0 / config.samples;
    int local_points = config.points / size;

    // Hooke's Law Physics Constant (Stiffness / Mass)
    double k_over_m = 6400.0; 

    // 2. Set up 1D Cartesian Topology for safe 2-way communication
    MPI_Comm cart_comm;
    int dims[1] = {size}; 
    int periods[1] = {0}; // Non-periodic (the ends don't wrap around)
    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, 0, &cart_comm);
    
    int left_neighbor, right_neighbor;
    MPI_Cart_shift(cart_comm, 0, 1, &left_neighbor, &right_neighbor);

    // 3. Allocate Memory (Physics requires position AND velocity)
    double* local_pos = (double*) malloc(local_points * sizeof(double));
    double* local_vel = (double*) malloc(local_points * sizeof(double));
    initialise_vector(local_pos, local_points, 0.0);
    initialise_vector(local_vel, local_points, 0.0);

    double* global_pos = NULL;
    FILE* out_file = NULL;

    if (rank == 0) {
        global_pos = (double*) malloc(config.points * sizeof(double));
        out_file = fopen(config.filepath, "w");
        if (out_file) print_header(&out_file, config.points);
    }

    // --- BENCHMARK START ---
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // 4. Main Simulation Loop
    for (int i = 0; i < time_steps; i++) {
        double current_time = i * dt;
        double left_ghost = 0.0;
        double right_ghost = 0.0;

        // --- 2-Way Ghost Cell Exchange ---
        // Send our left edge to left neighbor, receive right ghost from right neighbor
        MPI_Sendrecv(&local_pos[0], 1, MPI_DOUBLE, left_neighbor, 0,
                     &right_ghost, 1, MPI_DOUBLE, right_neighbor, 0, 
                     cart_comm, MPI_STATUS_IGNORE);
                     
        // Send our right edge to right neighbor, receive left ghost from left neighbor
        MPI_Sendrecv(&local_pos[local_points - 1], 1, MPI_DOUBLE, right_neighbor, 1,
                     &left_ghost, 1, MPI_DOUBLE, left_neighbor, 1, 
                     cart_comm, MPI_STATUS_IGNORE);

        // --- Physics Update ---
        for (int j = 0; j < local_points; j++) {
            // Boundary Condition 1: The Driver (Rank 0, index 0)
            if (rank == 0 && j == 0) {
                local_pos[j] = driver(current_time);
                local_vel[j] = 0.0; 
                continue;
            }
            // Boundary Condition 2: The Fixed Wall (Last Rank, last index)
            if (rank == size - 1 && j == local_points - 1) {
                local_pos[j] = 0.0;
                local_vel[j] = 0.0;
                continue;
            }

            // Figure out the neighbors for the finite difference equation
            double left_y  = (j == 0) ? left_ghost : local_pos[j - 1];
            double right_y = (j == local_points - 1) ? right_ghost : local_pos[j + 1];

            // Hooke's Law / Wave Equation
            double acceleration = k_over_m * (left_y - 2.0 * local_pos[j] + right_y);
            
            // Euler Integration
            local_vel[j] += acceleration * dt;
            local_pos[j] += local_vel[j] * dt;
        }

        // --- Aggregation & File I/O ---
        MPI_Gather(local_pos, local_points, MPI_DOUBLE, 
                   global_pos, local_points, MPI_DOUBLE, 
                   0, cart_comm);

        if (rank == 0 && out_file) {
            fprintf(out_file, "%d, %lf", i, current_time);
            for (int j = 0; j < config.points; j++) {
                fprintf(out_file, ", %lf", global_pos[j]);
            }
            fprintf(out_file, "\n");
        }
    }

    // --- BENCHMARK END ---
    double end_time = MPI_Wtime();
    double elapsed = end_time - start_time;

    // 5. Clean up & Final Report
    if (rank == 0) {
        printf("\n--- HOOKE'S LAW SIMULATION COMPLETED ---\n");
        printf("Processors used: %d\n", size);
        printf("Total points:    %d\n", config.points);
        printf("Wall-clock time: %f seconds\n", elapsed);
        printf("----------------------------------------\n");
        
        if (out_file) fclose(out_file);
        if (global_pos) free(global_pos);
    }

    free(local_pos); 
    free(local_vel);
    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}

// --- Helper Functions ---
SimConfig check_args(int argc, char **argv) {
    SimConfig config;
    if (argc != 5) {
        int rank; MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank == 0) printf("Usage: %s <points> <cycles> <samples> <output_filepath>\n", argv[0]);
        MPI_Finalize(); exit(-1);
    }
    config.points = atoi(argv[1]);
    config.cycles = atoi(argv[2]);
    config.samples = atoi(argv[3]);
    strncpy(config.filepath, argv[4], 255);
    return config;
}

void initialise_vector(double vector[], int size, double initial) { 
    for (int i = 0; i < size; i++) vector[i] = initial; 
}

void print_header(FILE** p_out_file, int points) {
    fprintf(*p_out_file, "#, time");
    for (int j = 0; j < points; j++) fprintf(*p_out_file, ", y[%d]", j);
    fprintf(*p_out_file, "\n");
}

double driver(double time) { 
    return sin(time * 2.0 * M_PI); 
}
