/*
 * test sorting network for n in [1; 24].
 * sort in ascending order
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define MAX_COMPS (127+1)

struct comparator
{
    int up;
    int down;
};

void fillin(int *v, int n, int value)
{
    for(int i=n-1; i>=0; --i)
    {
        v[i] = value % 2;
        value /= 2;
    }
}

void compare_and_swap(int *left, int *right)
{
    if (*left > *right)
    {
        int temp = *left;
        *left = *right;
        *right = temp;
    }
}

// BUG function
void sort_with_comps(int *v, int n, struct comparator *comps, int n_comps)
{
    for(int i=0; i<n_comps; ++i)
    {
        const int up = comps[i].up;
        const int down = comps[i].down;
        assert(up<n);
        assert(down<n);
        compare_and_swap(&v[up], &v[down]);
    }
}

int is_sorted(int *v, int n)
{
    for(int i=0; i<n-1; i++)
    {
        if (v[i] > v[i+1])  return 0;
    }
    return 1;
}

void print_vector(int *v, int n)
{
    for(int i=0; i<n; ++i)
        printf("%d ", v[i]);
    printf("\n");
}

int main()
{
    struct comparator comps[MAX_COMPS];
    FILE *f = fopen("res.txt", "r");
    if (f==NULL)
    {
        fprintf(stderr, "cannot open res.txt\n");
        exit(EXIT_FAILURE);
    }
    int n;
    fscanf(f, "%d %*d %*d\n", &n);
    int up, down;
    int index = 0;
    while(fscanf(f, "%d %d", &up, &down)==2)
    {
        comps[index].up = up;
        comps[index].down = down;
        ++index;
    }
    --index;
    if (index < 0)  return 0;
    const int n_comps = comps[index].up;
    const int n_tacts = comps[index].down;
    fclose(f);

    int * const v = malloc(n*sizeof(int));
    if (v == NULL)
    {
        fprintf(stderr, "cannot malloc %lu bytes\n", n*sizeof(int));
        exit(EXIT_FAILURE);
    }

    // all permutations
    for(int i=0; i<(1<<n); ++i)
    {
        /* uncomment to see nice output */
        //printf("-------i=%d-------\n", i);
        fillin(v, n, i);
        //print_vector(v, n);
        sort_with_comps(v, n, comps, n_comps);
        //print_vector(v, n);
        if (!is_sorted(v, n))
        {
            printf("not sorted, n=%d, i=%d\n", n, i);
        }
    }

    return 0;
}

