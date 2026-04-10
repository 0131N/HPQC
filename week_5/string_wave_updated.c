#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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
void update_positions(double* positions, int points, double time);
void generate_timestamps(double* time_stamps, int time_steps, double step_size);

int main(int argc, char **argv)
{
    // Parse command line arguments into our struct
    SimConfig config = check_args(argc, argv);

    // Calculate time steps based on arguments
    int time_steps = config.cycles * config.samples + 1; 
    double step_size = 1.0 / config.samples;

    // Create a vector for the time stamps in the data
    double* time_stamps = (double*) malloc(time_steps * sizeof(double));
    initialise_vector(time_stamps, time_steps, 0.0);
    generate_timestamps(time_stamps, time_steps, step_size);

    // Create a vector variable for the current positions
    double* positions = (double*) malloc(config.points * sizeof(double));
    initialise_vector(positions, config.points, 0.0);

    // Open file using the user-provided path
    FILE* out_file = fopen(config.filepath, "w");
    if (out_file == NULL) {
        printf("Error: Could not open file %s for writing.\n", config.filepath);
        return -1;
    }
    
    print_header(&out_file, config.points);

    // Iterate through each time step
    for (int i = 0; i < time_steps; i++)
    {
        update_positions(positions, config.points, time_stamps[i]);

        fprintf(out_file, "%d, %lf", i, time_stamps[i]);
        for (int j = 0; j < config.points; j++)
        {
            fprintf(out_file, ", %lf", positions[j]);
        }
        fprintf(out_file, "\n");
    }

    // Free allocated memory
    free(time_stamps);
    free(positions);
    fclose(out_file);

    printf("Simulation complete. Data written to %s\n", config.filepath);

    return 0;
}

// Parses arguments and returns a populated struct
SimConfig check_args(int argc, char **argv) {
    SimConfig config;
    
    // Check if we have the correct number of arguments (Program name + 4 args)
    if (argc != 5) {
        printf("Usage: %s <points> <cycles> <samples> <output_filepath>\n", argv[0]);
        printf("Example: %s 50 5 25 data/string_wave.csv\n", argv[0]);
        exit(-1);
    }

    config.points = atoi(argv[1]);
    config.cycles = atoi(argv[2]);
    config.samples = atoi(argv[3]);
    strncpy(config.filepath, argv[4], sizeof(config.filepath) - 1);
    config.filepath[sizeof(config.filepath) - 1] = '\0'; // Ensure null-termination

    // Basic validation
    if (config.points <= 0 || config.cycles <= 0 || config.samples <= 0) {
        printf("Error: Points, cycles, and samples must be greater than 0.\n");
        exit(-1);
    }

    return config;
}

void initialise_vector(double vector[], int size, double initial) {
    for (int i = 0; i < size; i++) {
        vector[i] = initial;
    }
}

void generate_timestamps(double* time_stamps, int time_steps, double step_size) {
    for (int i = 0; i < time_steps; i++) {
        time_stamps[i] = i * step_size;
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

// Completing the cut-off function: Simple follow-the-leader model
void update_positions(double* positions, int points, double time) {
    double* new_positions = (double*) malloc(points * sizeof(double));

    new_positions[0] = driver(time);
    
    // Each point takes the position of the one before it
    for (int i = 1; i < points; i++) {
        new_positions[i] = positions[i-1];
    }

    // Copy new positions back to the main array
    for (int i = 0; i < points; i++) {
        positions[i] = new_positions[i];
    }

    free(new_positions);
}

