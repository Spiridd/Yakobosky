#include "point.h"
#include <stdio.h>
#include <stdlib.h>

int compare_points(const void *left, const void *right)
{
    // if left < right return -1
    // if left > right return +1
    // else            return 0
    // points with negative index are considered bigger
    const int index1 = ((struct Point*)left)->index;
    const int index2 = ((struct Point*)right)->index;

    if (index2 < 0)         return -1;
    else if (index1 < 0)    return +1;

    static const float eps = 1e-6;
    const float x1 = ((struct Point*)left)->coord[0];
    const float x2 = ((struct Point*)right)->coord[0];
    if (x1 < x2)    return -1;
    else
    {
        const float dif = (x1-x2)/x1;
        if (dif<eps && dif>-eps)    return 0;
        else return 1;
    }
}

void read_points(const char *filename, struct Point **points_ptr, int *size)
{
    FILE *f = fopen(filename, "rb");
    if (f==NULL)
    {
        fprintf(stderr, "cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fread(size, sizeof(int), 1, f);
    *points_ptr = malloc(*size * sizeof(struct Point));
    struct Point * const points = *points_ptr;
    fread(points, sizeof(struct Point), *size, f);
    fclose(f);
}

void write_points(const char *filename, struct Point *points, int size)
{
    FILE *f = fopen(filename, "wb");
    if (f==NULL)
    {
        fprintf(stderr, "cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fwrite(&size, sizeof(int), 1, f);
    fwrite(points, sizeof(struct Point), size, f);
    fclose(f);
}

void print_points(struct Point *points, int n)
{
    for(int i=0; i<n; ++i)
    {
        struct Point p = points[i];
        printf("%f\t", p.coord[0]);
    }
    printf("\n");
}
