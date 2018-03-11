/*
 * generates M*N "Point Structures"
 * and prints them into a file argv[3]
 */
#include <stdio.h>
#include <stdlib.h>
#include "point.h"

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s N M file\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    const int N = (int) strtol(argv[1], NULL, 10);
    const int M = (int) strtol(argv[2], NULL, 10);
    struct Point *points = malloc(N*M*sizeof(struct Point));
    for(int i=0; i<N; ++i)
    {
        for(int j=0; j<M; ++j)
        {
            const int index = i*M+j;
            points[index].index = index;
            points[index].coord[0] = (double)rand()/RAND_MAX;
            points[index].coord[1] = (double)rand()/RAND_MAX;
        }
    }
    FILE *f = fopen(argv[3], "wb");
    if (f==NULL)
    {
        fprintf(stderr, "cannot open %s\n", argv[3]);
        exit(EXIT_FAILURE);
    }
    const int npoints = N*M;
    fwrite(&npoints, sizeof(int), 1, f);
    fwrite(points, sizeof(struct Point), N*M, f);
    fclose(f);
    free(points);

    return 0;
}

