/*
 * Generates Batcher's sorting network
 * and prints it into stdin
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int comps = 0;

void process_comparator(int up, int down, int *buf)
{
    printf("%d %d\n", up, down);
    ++comps;
    const int tacts = buf[up] >= buf[down] ? buf[up] : buf[down];
    buf[up] = buf[down] = tacts+1;
}

int generate_single_swap_network(int first, int size, int stride, int *buf)
{
    int up = first;
    int down = up+stride;
    const int max_number = first+size*stride;
    while (down < max_number)
    {
        process_comparator(up, down, buf);
        up = down+stride;
        down = up+stride;
    }
    return up < max_number ? up : -1;
}

void generate_swap_network(int first_left, int size_left, int first_right,
        int size_right, int stride, int *buf)
{
    assert(size_left*size_right > 1);
    int up = generate_single_swap_network(first_left+stride, size_left-1, stride, buf);
    if (up < 0) generate_single_swap_network(first_right, size_right, stride, buf);
    else
    {
        process_comparator(up, first_right, buf);
        generate_single_swap_network(first_right+stride, size_right-1, stride, buf);
    }
}

void generate_merge_network(int first_left, int size_left, int first_right,
        int size_right, int stride, int *buf)
{
    if (size_left*size_right <= 0)  return;
    if (size_left*size_right == 1)
    {
        process_comparator(first_left, first_right, buf);
        return;
    }
    // quantity of odd numbers in left part
    const int n = (size_left-1)/2 + 1;
    // quantity of odd numbers in right part
    const int m = (size_right-1)/2 + 1;
    // merge odd
    generate_merge_network(first_left, n, first_right, m, 2*stride, buf);
    // merge even
    generate_merge_network(first_left+stride, size_left-n, first_right+stride, size_right-m, 2*stride, buf);
    // compare and swap
    generate_swap_network(first_left, size_left, first_right, size_right, stride, buf);
}

void generate_sorting_network(int from, int n, int *buf)
{
    if (n <= 1) return;
    const int size_left = (n-1)/2 + 1;
    const int size_right = n-size_left;
    generate_sorting_network(from, size_left, buf);
    generate_sorting_network(from+size_left, size_right, buf);
    generate_merge_network(from, size_left, from+size_left, size_right, 1, buf);
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

void init_buf(int **buf_ptr, int n)
{
    *buf_ptr = malloc(n*sizeof(int));
    int * const buf = *buf_ptr;
    if (buf == NULL)
    {
        fprintf(stderr, "cannot malloc %lu bytes\n", n*sizeof(int));
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<n; ++i)  buf[i] = 0;
}

void sort(int n)
{
    assert(n>=0);
    printf("%d 0 0\n", n);

    int *buf;
    init_buf(&buf, n);
    generate_sorting_network(0, n, buf);

    printf("%d\n", comps); // global variable
    int tacts = 0;
    for(int i=0; i<n; ++i)
    {
        if (buf[i] > tacts) tacts = buf[i];
    }
    printf("%d\n", tacts);
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

