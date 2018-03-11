/*
 * reads data from file argv[1] and sorts it along x axis
 * with the fastest serial algorithm.
 * Prints sorting time into stdout
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "point.h"
#include "sort.h"

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int n_proc;
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    if (n_proc > 1)
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank==0)    fprintf(stderr, "No. of processes must be 1.\n");
        exit(EXIT_FAILURE);
    }

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct Point *points;
    int size;
    read_points(argv[1], &points, &size);

    const double begin = MPI_Wtime();
    struct Point *buf = safe_malloc(size/2*sizeof(struct Point));
    serial_sort(points, size, buf, &compare_points);
    //qsort(qpoints, size, sizeof(struct Point), &compare_points);
    const double end = MPI_Wtime();
    const double time = end-begin;
    printf("%f\n0\n", time);

    write_points(argv[2], points, size);
    free(points);
    free(buf);

    MPI_Finalize();
    return 0;
}

