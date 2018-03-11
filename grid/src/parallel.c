/*
 * Reads data from argv[1], sorts it with Batcher's sort
 * in parallel and then prints sorted data into argv[2].
 * No checking is performed
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include "point.h"
#include "sort.h"

// 1 <= n <= 128
#define MAX_COMPARATORS 1471

struct Comparator
{
    int up;
    int down;
};

void process_comparator(int up, int down, struct Comparator *comparators,
                        int *n_comparators)
{
    const struct Comparator c = {up, down};
    comparators[(*n_comparators)++] = c;
}

int generate_single_swap_network(int first, int size, int stride,
                                 struct Comparator *comparators, int *n_comparators)
{
    int up = first;
    int down = up+stride;
    const int max_number = first+size*stride;
    while (down < max_number)
    {
        process_comparator(up, down, comparators, n_comparators);
        up = down+stride;
        down = up+stride;
    }
    return up < max_number ? up : -1;
}

void generate_swap_network(int first_left, int size_left, int first_right,
        int size_right, int stride, struct Comparator *comparators, int *n_comparators)
{
    assert(size_left*size_right > 1);
    const int up = generate_single_swap_network(first_left+stride, size_left-1,
                                                stride, comparators, n_comparators);
    if (up < 0)
    {
        generate_single_swap_network(first_right, size_right, stride,
                                     comparators, n_comparators);
    }
    else
    {
        process_comparator(up, first_right, comparators, n_comparators);
        generate_single_swap_network(first_right+stride, size_right-1, stride,
                                     comparators, n_comparators);
    }
}

void generate_merge_network(int first_left, int size_left, int first_right,
        int size_right, int stride, struct Comparator *comparators, int *n_comparators)
{
    if (size_left*size_right <= 0)  return;
    if (size_left*size_right == 1)
    {
        process_comparator(first_left, first_right, comparators, n_comparators);
        return;
    }
    // quantity of odd numbers in left part
    const int n = (size_left-1)/2 + 1;
    // quantity of odd numbers in right part
    const int m = (size_right-1)/2 + 1;
    // merge odd
    generate_merge_network(first_left, n, first_right, m, 2*stride, comparators,
                           n_comparators);
    // merge even
    generate_merge_network(first_left+stride, size_left-n, first_right+stride,
                           size_right-m, 2*stride, comparators, n_comparators);
    // compare and swap
    generate_swap_network(first_left, size_left, first_right, size_right, stride,
                          comparators, n_comparators);
}

void generate_sorting_network(int from, int n, struct Comparator *comparators,
                              int *n_comparators)
{
    if (n <= 1) return;
    const int size_left = (n-1)/2 + 1;
    const int size_right = n-size_left;
    generate_sorting_network(from, size_left, comparators, n_comparators);
    generate_sorting_network(from+size_left, size_right, comparators,
                             n_comparators);
    generate_merge_network(from, size_left, from+size_left, size_right, 1,
                           comparators, n_comparators);
}

void distribute_tasks(int n_tasks, int n_proc, int **tasks_ptr, int **from_ptr,
                      int **to_ptr)
{
    /* distributing tasks in a following manner:
     * (4 processes, 22 tasks)
     * 0    1   2   3   ranks
     *
     * 5    5   5   5   not all tasks
     * 6    6   5   5   all the tasks (with remained)
     */
    const int tpp = (n_tasks-1)/n_proc + 1; // max tasks per process
    const int remained = n_tasks - (tpp-1)*n_proc;
    const int min_task_quantity = (tpp > 0 ? tpp-1 : 0);
    *tasks_ptr = safe_malloc(n_proc*sizeof(int));
    int * const tasks = *tasks_ptr;
    for(int i=0; i<n_proc; ++i) tasks[i] = min_task_quantity;
    for(int i=0; i<remained; ++i) ++tasks[i];

    *from_ptr = safe_malloc(n_proc*sizeof(int));
    *to_ptr = safe_malloc(n_proc*sizeof(int));
    int * const from = *from_ptr;
    int * const to = *to_ptr;
    from[0] = 0;
    to[0] = tasks[0];
    for(int i=1; i<n_proc; ++i)
    {
        from[i] = to[i-1];
        to[i] = from[i] + tasks[i];
    }
}

void join(struct Point *a, struct Point *b, struct Point *c, int n, int myrank,
          int pair_rank, compare_func compare)
{
    if (myrank < pair_rank)
    {
        for(int ia=0, ib=0, k=0; k<n; )
        {
            if (compare(&a[ia], &b[ib])<0)              c[k++] = a[ia++];
            else                                        c[k++] = b[ib++];
        }
    }
    else
    {
        for(int ia=n-1, ib=n-1, k=n-1; k>=0; )
        {
            if (compare(&a[ia], &b[ib])>0)              c[k--] = a[ia--];
            else                                        c[k--] = b[ib--];
        }
    }
}

void read_data(const char *filename, int *total, struct Point **points_ptr,
               int *n, int rank, int n_proc, MPI_Datatype MPI_Point)
{
    MPI_File input_file;
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL,
                  &input_file);
    MPI_File_set_view(input_file, 0, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
    MPI_File_read_at_all(input_file, 0, total, 1, MPI_INT, MPI_STATUS_IGNORE);
    
    int *from, *to, *tasks;
    distribute_tasks(*total, n_proc, &tasks, &from, &to);
    *n = tasks[rank];
    MPI_Datatype MPI_points_batch;
    const int displ = from[rank];
    MPI_Type_create_indexed_block(1, *n, &displ, MPI_Point,
                                  &MPI_points_batch);
    MPI_Type_commit(&MPI_points_batch);

    MPI_File_set_view(input_file, 1*sizeof(int), MPI_Point, MPI_points_batch,
                      "native", MPI_INFO_NULL);
    *points_ptr = safe_malloc((*n)*sizeof(struct Point));
    MPI_File_read_at_all(input_file, 0, *points_ptr, *n, MPI_Point,
                         MPI_STATUS_IGNORE);
    MPI_File_close(&input_file);

    free(from);
    free(to);
    free(tasks);
    MPI_Type_free(&MPI_points_batch);
}

void add_dummies(struct Point **points_ptr, int n, int total, int n_proc, int *chunk_size)
{
    *chunk_size = (total-1)/n_proc+1;
    if (n < *chunk_size)
    {
        // add points with index=-1 to the end
        *points_ptr = realloc(*points_ptr, (*chunk_size)*sizeof(struct Point));
        for(int i=n; i<*chunk_size; ++i)
        {
            const struct Point c = {.index = -1};
            (*points_ptr)[i] = c;
        }
    }
}

void batcher(struct Comparator *comparators, int n_comparators, struct Point **points_ptr,
             int chunk_size, int rank, MPI_Datatype MPI_Point, compare_func compare)
{
    struct Point *points = *points_ptr;
    struct Point *recvbuf   = safe_malloc(chunk_size*sizeof(struct Point));
    struct Point *merge_buf = safe_malloc(chunk_size*sizeof(struct Point));
    for(int i=0; i<n_comparators; ++i)
    {
        struct Comparator c = comparators[i];
        if (rank!=c.up && rank!=c.down) continue;
        
        MPI_Request send_request, recv_request;
        const int dest = rank==c.up ? c.down : c.up;
        MPI_Isend(points, chunk_size, MPI_Point, dest, 0, MPI_COMM_WORLD,
                  &send_request);
        MPI_Irecv(recvbuf, chunk_size, MPI_Point, dest, 0, MPI_COMM_WORLD,
                  &recv_request);
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
        MPI_Wait(&send_request, MPI_STATUS_IGNORE);

        join(points, recvbuf, merge_buf, chunk_size, rank, dest, compare);
        struct Point *tmp = points;
        points = merge_buf;
        merge_buf = tmp;
    }
    *points_ptr = points;
    free(merge_buf);
    free(recvbuf);
}

void write_data(const char *filename, struct Point *points, int chunk_size,
                int total, MPI_Datatype MPI_Point)
{
    int dummies = 0;
    for(int i=chunk_size-1; i>=0; --i)
    {
        if (points[i].index < 0)    ++dummies;
        else                        break;
    }
    const int not_dummies = chunk_size-dummies;
    int start = 0;
    MPI_Exscan(&not_dummies, &start, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    MPI_File outfile;
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE|MPI_MODE_WRONLY,
                  MPI_INFO_NULL, &outfile);
    MPI_File_set_view(outfile, 0, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
    MPI_File_write_at_all(outfile, 0, &total, 1, MPI_INT, MPI_STATUS_IGNORE);

    MPI_Datatype MPI_points_batch;
    MPI_Type_create_indexed_block(1, not_dummies, &start, MPI_Point, &MPI_points_batch);
    MPI_Type_commit(&MPI_points_batch);
    MPI_File_set_view(outfile, 1*sizeof(int), MPI_Point, MPI_points_batch,
                      "native", MPI_INFO_NULL);

    MPI_File_write_at_all(outfile, 0, points, not_dummies, MPI_Point,
                          MPI_STATUS_IGNORE);
    MPI_File_close(&outfile);
    MPI_Type_free(&MPI_points_batch);
}

int get_tacts(struct Comparator *comparators, int n_comparators, int n_proc)
{
    int *tacts = malloc(n_proc*sizeof(int));
    for(int i=0; i<n_proc; ++i) tacts[i] = 0;
    int max_tact = 0;
    for(int i=0; i<n_comparators; ++i)
    {
        const struct Comparator c = comparators[i];
        const int up = c.up;
        const int down = c.down;
        const int current = tacts[up] >= tacts[down] ? tacts[up] : tacts[down];
        tacts[up] = tacts[down] = current + 1;
        if (max_tact < current+1)   max_tact = current+1;
    }
    free(tacts);
    return max_tact;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int n_proc;
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    if (n_proc == 1)
    {
        if (rank==0)    fprintf(stderr, "The program is not supposed to be running on one node. If this is the very option you want, run sort_seq\n");
        exit(EXIT_FAILURE);
    }
    if (n_proc > 128)
    {
        if (rank==0)    fprintf(stderr, "The program is not designed to run more than 128 processes\n");
        exit(EXIT_FAILURE);
    }

    if (argc != 3)
    {
        if (rank==0)    fprintf(stderr, "Usage: %s input_file output_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // create MPI type for struct Point
    MPI_Datatype MPI_Point;
    const int blocklens[] = {2, 1};
    const MPI_Aint displs[] = {0, 2*sizeof(float)};
    const MPI_Datatype types[] = {MPI_FLOAT, MPI_INT};
    MPI_Type_create_struct(2, blocklens, displs, types, &MPI_Point);
    MPI_Type_commit(&MPI_Point);

    // read data from file
    int total, n;
    struct Point *points;
    read_data(argv[1], &total, &points, &n, rank, n_proc, MPI_Point);

    const double begin = MPI_Wtime();

    // sort pieces in parallel (one thread)
    //! n/2 or n in buf?
    struct Point * const buf = safe_malloc(n/2*sizeof(struct Point));
    serial_sort(points, n, buf, &compare_points);
    free(buf);
    
    // add dummy points to have equal chunks on each processor
    int chunk_size;
    add_dummies(&points, n, total, n_proc, &chunk_size);

    // generate network - create vector of comparators
    struct Comparator comparators[MAX_COMPARATORS];
    int n_comparators = 0;
    generate_sorting_network(0, n_proc, comparators, &n_comparators);

    // perform Batcher's sort
    batcher(comparators, n_comparators, &points, chunk_size, rank, MPI_Point, &compare_points);

    const double end = MPI_Wtime();
    const double mytime = end-begin;
    double max_time;
    MPI_Reduce(&mytime, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    const int max_tact = get_tacts(comparators, n_comparators, n_proc);
    if (rank==0)    printf("%f\n%d\n", max_time, max_tact);

    // write data to file
    write_data(argv[2], points, chunk_size, total, MPI_Point);

    free(points);
    MPI_Type_free(&MPI_Point);

    MPI_Finalize();
    return 0;
}

