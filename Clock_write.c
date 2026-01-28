#include <stdio.h>
#include <time.h>

int main() {
    FILE *fp;
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    fp = fopen("data/test_output.txt", "w");


    for (int i = 0; i < 8000000; i++) {
        fprintf(fp, "Line number %d\n", i);
    }

    fclose(fp);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to write: %f seconds\n", cpu_time_used);

    return 0;
}

