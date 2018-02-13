/*
 * reads data from file and sort it along x axis
 * with the fastest serial algorithm
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define HEAP_THRESHOLD 50000
#define MERGE_THRESHOLD 22

struct Point
{
    float coord[2];
    int index;
};

typedef int (*Comparator)(const void*, const void*);

int compare_points(const void *left, const void *right)
{
    static const float eps = 1e-6;
    // if left <= right return +1
    // else return -1
    const float x1 = ((struct Point*)left)->coord[0];
    const float x2 = ((struct Point*)right)->coord[0];
    if (x1 < x2)    return -1;
    else
    {
        const float f = (x1-x2)/x1;
        if (f<eps && f>-eps)    return 0;
        else return 1;
    }
}

void insertion_sort(struct Point *points, int n, Comparator comparator)
{
    for(int i=1; i<n; ++i)
    {
        const struct Point t = points[i];
        int pos = i;
        while (pos > 0 && (comparator(&t, &points[pos-1])<0))
        {
            points[pos] = points[pos-1];
            --pos;
        }
        points[pos] = t;
    }
}

int parent_index(int index)
{
    return index/2;
}

int left_child_index(int index)
{
    return 2*index+1;
}

int right_child_index(int index)
{
    return 2*index+2;
}

int is_left(int index, int size)
{
    return left_child_index(index) >= size;
}

void sift_down(struct Point *points, int n, int index, Comparator comparator)
{
    // smallest element goes down
    while(1)
    {
        int largest = index;
        const int left = left_child_index(index);
        if (left < n && (comparator(&points[largest], &points[left])<0))
            largest = left;
        const int right = right_child_index(index);
        if (right < n && (comparator(&points[largest], &points[right])<0))
            largest = right;
        if (largest != index)
        {
            // swap
            struct Point t = points[largest];
            points[largest] = points[index];
            points[index] = t;
            index = largest;
        }
        else    break;
    }
}

void swap_first_and_last(struct Point *points, int n, Comparator comparator)
{
    // swaps first and last elements, then sifts down the first one
    struct Point t = points[0];
    points[0] = points[n-1];
    points[n-1] = t;
    sift_down(points, n-1, 0, comparator);
}

void heap_sort(struct Point *points, int n, Comparator comparator)
{
    // 1. make heap
    // 2. swap first and last elements, n=n-1
    // 3. sift down the first one (heapify)
    // 4. repeat from 2 until n = 1 
    const int start = parent_index(n-1);
    for(int i=start; i>=0; --i)
    {
        sift_down(points, n, i, comparator);
    }
    for(int i=0; i<n-1; ++i)
    {
        swap_first_and_last(points, n-i, comparator);
    }
}

void merge(struct Point *points, int n, int m, Comparator comparator)
{
    struct Point *buf = malloc(n*sizeof(struct Point));
    if (buf==NULL)
    {
        fprintf(stderr, "cannot malloc\n");
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<n; ++i)
        buf[i] = points[i];
    int i1 = 0;
    int i2 = 0;
    while(i1 < n && i2 < m)
    {
        if (comparator(&buf[i1], &points[n+i2])<=0)
        {
            points[i1+i2] = buf[i1];
            ++i1;
        }
        else
        {
            points[i1+i2] = points[n+i2];
            ++i2;
        }
    }
    while (i1 < n)
    {
        // i2 == m
        points[i1+i2] = buf[i1];
        ++i1;
    }
    free(buf);
    /*
    while (i2 < m)
    {
        // i1 == n
        points[i1+i2] = points[n+i2];
        ++i2;
    }
    */
}

void sort(struct Point *points, int n, Comparator comparator)
{
    //if (n < HEAP_THRESHOLD) heap_sort(points, n, comparator);
    if (n < MERGE_THRESHOLD) insertion_sort(points, n, comparator);
    else
    {
        // divide and merge
        sort(points+0, n/2, comparator);
        sort(points+n/2, n-n/2, comparator);
        merge(points, n/2, n-n/2, comparator);
    }
}

int is_sorted(struct Point *points, int n, Comparator comparator)
{
    for(int i=0; i<n-1; ++i)
    {
        if (comparator(&points[i], &points[i+1]) > 0)
        {
            printf("sorting error: i=%d, %f %f\n", i, points[i].coord[0], points[i+1].coord[0]);
            return 0;
        }
    }
    return 1;
}

void read_data(const char *filename, struct Point **points_ptr, int *size)
{
    FILE *f = fopen(filename, "rb");
    if (f==NULL)
    {
        fprintf(stderr, "cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fread(size, sizeof(int), 1, f);
    *points_ptr = malloc(*size * sizeof(struct Point));
    if (*points_ptr==NULL)
    {
        fprintf(stderr, "cannot malloc\n");
        exit(EXIT_FAILURE);
    }
    struct Point * const points = *points_ptr;
    fread(points, sizeof(struct Point), *size, f);
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct Point *points;
    int size;
    read_data(argv[1], &points, &size);

    // copy data to sort with qsort
    struct Point *qpoints = malloc(size*sizeof(struct Point));
    if (qpoints==NULL)  return 1;
    for(int i=0; i<size; ++i)
        qpoints[i] = points[i];

    const clock_t begin1 = clock();
    sort(points, size, &compare_points);
    const clock_t end1 = clock();
    const double time1 = (double)(end1-begin1)/CLOCKS_PER_SEC;
    printf("DHsort = %f s\n", time1);
    is_sorted(points, size, &compare_points);

    const clock_t begin2 = clock();
    qsort(qpoints, size, sizeof(struct Point), &compare_points);
    const clock_t end2 = clock();
    const double time2 = (double)(end2-begin2)/CLOCKS_PER_SEC;
    printf("qsort  = %f s\n", time2);
    is_sorted(qpoints, size, &compare_points);

    free(points);
    free(qpoints);

    return 0;
}

