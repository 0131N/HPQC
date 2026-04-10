#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    FILE *fp;
    clock_t start, end;
    double cpu_time_used;
    char buffer[100];

    start = clock();

    fp = fopen("data/test_output.txt", "r");

    }


    while (fgets(buffer, sizeof(buffer), fp) != NULL) {

    }

    fclose(fp);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to read: %f seconds\n", cpu_time_used);

    return 0;
}
