#ifndef POINT_H
#define POINT_H

typedef int (*compare_func)(const void*, const void*);

struct Point
{
    float coord[2];
    int index;
};

int compare_points(const void *left, const void *right);
void read_points(const char *filename, struct Point **points_ptr, int *size);
void write_points(const char *filename, struct Point *points, int size);
void print_points(struct Point *points, int n);

#endif
