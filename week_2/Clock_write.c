#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    FILE *fp;
    clock_t start, end;
    double cpu_time_used;

    if (argc != 2) {
        printf("Usage: %s <number_of_lines>\n", argv[0]);
        return 1;
    }

    int num_lines = atoi(argv[1]);

    start = clock();

    fp = fopen("data/test_output.txt", "w");
    if (fp == NULL) {
       perror("Error opening file"); // This tells you WHY it failed (e.g., "No such file or directory")
       return 1;
}
    for (int i = 0; i < num_lines; i++) {
        fprintf(fp, "Line number %d\n", i);
    }

    fclose(fp);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to write %d lines: %f seconds\n", num_lines, cpu_time_used);

    return 0;
}
