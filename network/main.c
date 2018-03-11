/*
 * Generates Batcher's sorting network
 * and prints it into stdin
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void process_comparator(int up, int down, int *tacts, int *n_comparators)
{
    printf("%d %d\n", up, down);
    ++(*n_comparators);
    const int current = tacts[up] >= tacts[down] ? tacts[up] : tacts[down];
    tacts[up] = tacts[down] = current+1;
}

int generate_single_swap_network(int first, int size, int stride, int *tacts, int *n_comparators)
{
    int up = first;
    int down = up+stride;
    const int max_number = first+size*stride;
    while (down < max_number)
    {
        process_comparator(up, down, tacts, n_comparators);
        up = down+stride;
        down = up+stride;
    }
    return up < max_number ? up : -1;
}

void generate_swap_network(int first_left, int size_left, int first_right,
        int size_right, int stride, int *tacts, int *n_comparators)
{
    assert(size_left*size_right > 1);
    const int up = generate_single_swap_network(first_left+stride, size_left-1, stride, tacts, n_comparators);
    if (up < 0) generate_single_swap_network(first_right, size_right, stride, tacts, n_comparators);
    else
    {
        process_comparator(up, first_right, tacts, n_comparators);
        generate_single_swap_network(first_right+stride, size_right-1, stride, tacts, n_comparators);
    }
}

void generate_merge_network(int first_left, int size_left, int first_right,
        int size_right, int stride, int *tacts, int *n_comparators)
{
    if (size_left*size_right == 0)  return;
    if (size_left*size_right == 1)
    {
        process_comparator(first_left, first_right, tacts, n_comparators);
        return;
    }
    // quantity of odd numbers in left part
    const int n = (size_left-1)/2 + 1;
    // quantity of odd numbers in right part
    const int m = (size_right-1)/2 + 1;
    // merge odd
    generate_merge_network(first_left, n, first_right, m, 2*stride, tacts, n_comparators);
    // merge even
    generate_merge_network(first_left+stride, size_left-n, first_right+stride, size_right-m, 2*stride, tacts, n_comparators);
    // compare and swap
    generate_swap_network(first_left, size_left, first_right, size_right, stride, tacts, n_comparators);
}

void generate_sorting_network(int from, int n, int *tacts, int *n_comparators)
{
    if (n == 1) return;
    const int size_left = (n-1)/2 + 1;
    const int size_right = n-size_left;
    generate_sorting_network(from, size_left, tacts, n_comparators);
    generate_sorting_network(from+size_left, size_right, tacts, n_comparators);
    generate_merge_network(from, size_left, from+size_left, size_right, 1, tacts, n_comparators);
}

/*
void merge(int size_left, int size_right)
{
    static const int starting_number = 0;
    static const int min_stride = 1;
    printf("%d %d 0\n", size_left, size_right);
    generate_merge_network(starting_number, size_left,
                 starting_number+size_left, size_right, min_stride);
}
*/

void init_tacts(int **tacts_ptr, int n)
{
    *tacts_ptr = malloc(n*sizeof(int));
    int * const tacts = *tacts_ptr;
    if (tacts == NULL)
    {
        fprintf(stderr, "cannot malloc %lu bytes\n", n*sizeof(int));
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<n; ++i)  tacts[i] = 0;
}

void sort(int n)
{
    assert(n>=0);
    printf("%d 0 0\n", n);

    int *tacts;
    init_tacts(&tacts, n);
    int n_comparators = 0;
    generate_sorting_network(0, n, tacts, &n_comparators);
    printf("%d\n", n_comparators);

    int max_tacts = 0;
    for(int i=0; i<n; ++i)
    {
        if (tacts[i] > max_tacts) max_tacts = tacts[i];
    }
    printf("%d\n", max_tacts);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s n\n"
                        "where n is a positive number\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int n = (int) strtol(argv[1], NULL, 10);
    sort(n);

    return 0;
}

