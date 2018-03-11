#ifndef SORT_H
#define SORT_H

#include "point.h"

#define HEAP_THRESHOLD 50000
#define MERGE_THRESHOLD 22

void* safe_malloc(int nbytes);
void insertion_sort(struct Point *points, int n, compare_func comparator);
void sift_down(struct Point *points, int n, int index, compare_func comparator);
void swap_first_and_last(struct Point *points, int n, compare_func comparator);
void heap_sort(struct Point *points, int n, compare_func comparator);
void merge(struct Point *points, int n, int m, struct Point *buf, compare_func comparator);
void serial_sort(struct Point *points, int n, struct Point *buf, compare_func comparator);
int is_sorted(struct Point *points, int n, compare_func comparator);

#endif
