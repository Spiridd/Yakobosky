/*
 * reads data from file argv[1] and checks if it is sorted.
 * Prints result into stdout
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "point.h"
#include "sort.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct Point *points;
    int size;
    read_points(argv[1], &points, &size);
    //print_points(points, size);
    if (is_sorted(points, size, &compare_points))   printf("Sorted.\n");
    free(points);

    return 0;
}
