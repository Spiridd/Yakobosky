#include <stdio.h>
#include <stdlib.h>
#include "sort.h"

void serial_sort(struct Point *points, int n, struct Point *buf, compare_func compare)
{
//    qsort(points, n, sizeof(struct Point), &compare_points);

    //if (n < HEAP_THRESHOLD) heap_sort(points, n, compare);
    if (n < MERGE_THRESHOLD) insertion_sort(points, n, compare);
    else
    {
        // divide and merge
        serial_sort(points+0, n/2, buf, compare);
        serial_sort(points+n/2, n-n/2, buf, compare);
        merge(points, n/2, n-n/2, buf, compare);
    }
}

void* safe_malloc(int nbytes)
{
    void* ret = malloc(nbytes);
    if (ret==NULL)
    {
        // all processes can print
        fprintf(stderr, "cannot malloc\n");
        exit(EXIT_FAILURE);
    }
    return ret;
}

void insertion_sort(struct Point *points, int n, compare_func compare)
{
    for(int i=1; i<n; ++i)
    {
        const struct Point t = points[i];
        int pos = i;
        while (pos > 0 && (compare(&t, &points[pos-1])<0))
        {
            points[pos] = points[pos-1];
            --pos;
        }
        points[pos] = t;
    }
}

void sift_down(struct Point *points, int n, int index, compare_func compare)
{
    // smallest element goes down
    while(1)
    {
        int largest = index;
        const int left = 2*index+1;
        if (left < n && (compare(&points[largest], &points[left])<0))
            largest = left;
        const int right = left+1;
        if (right < n && (compare(&points[largest], &points[right])<0))
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

void swap_first_and_last(struct Point *points, int n, compare_func compare)
{
    // swaps first and last elements, then sifts down the first one
    struct Point t = points[0];
    points[0] = points[n-1];
    points[n-1] = t;
    sift_down(points, n-1, 0, compare);
}

void heap_sort(struct Point *points, int n, compare_func compare)
{
    // 1. make heap
    // 2. swap first and last elements, n=n-1
    // 3. sift down the first one (heapify)
    // 4. repeat from 2 until n = 1 
    const int start = (n-1)/2;
    for(int i=start; i>=0; --i)
    {
        sift_down(points, n, i, compare);
    }
    for(int i=0; i<n-1; ++i)
    {
        swap_first_and_last(points, n-i, compare);
    }
}

void merge(struct Point *points, int n, int m, struct Point *buf, compare_func compare)
{
    for(int i=0; i<n; ++i)
        buf[i] = points[i];
    int i1 = 0;
    int i2 = 0;
    while(i1 < n && i2 < m)
    {
        if (compare(&buf[i1], &points[n+i2])<=0)
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
}

int is_sorted(struct Point *points, int n, compare_func compare)
{
    for(int i=0; i<n-1; ++i)
    {
        if (compare(&points[i], &points[i+1]) > 0)
        {
            printf("sorting error: i=%d, %f %f\n", i, points[i].coord[0], points[i+1].coord[0]);
            return 0;
        }
    }
    return 1;
}
