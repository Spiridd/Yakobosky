#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#define MERGESORT_THRESHOLD 22

// use 64 bit indexes
typedef int64_t idx_t;

// sort idx_t type data

int insertion_sort(idx_t * const v, int n)
{
    for(int i=1; i<n; ++i)
    {
        const idx_t t = v[i];
        int pos = i;
        while (pos > 0 && t < v[pos-1])
        {
            v[pos] = v[pos-1];
            --pos;
        }
        v[pos] = t;
    }
    return 0;
}

int plain_sort(idx_t * const v, int n)
{
    return insertion_sort(v, n);
}

int merge(idx_t * const v, idx_t begin, idx_t mid, idx_t end, idx_t * const buf)
{
    const idx_t n1 = mid - begin;
    for(idx_t i=0; i<n1; ++i)
        buf[i] = v[begin+i];
    const idx_t n2 = end - mid;
    idx_t i1 = 0;
    idx_t i2 = 0;
    while (i1 < n1 && i2 < n2)
    {
        if (buf[i1] <= v[mid+i2])
        {
            v[begin+i1+i2] = buf[i1];
            ++i1;
        }
        else
        {
            v[begin+i1+i2] = v[mid+i2];
            ++i2;
        }
    }

    while (i1 < n1)
    {
        v[begin+i1+i2] = buf[i1];
        ++i1;
    }
    while (i2 < n2)
    {
        v[begin+i1+i2] = v[mid+i2];
        ++i2;
    }
    return 0;
}

int mergesort(idx_t * const v, idx_t begin, idx_t end, idx_t * const buf)
{
    // v is array indexed as [begin, end)
    const idx_t n = end - begin;
    if (n <= MERGESORT_THRESHOLD)
    {
        return plain_sort(v+begin, n);
    }

    const idx_t mid = (begin + end) / 2;
    if (mergesort(v, begin, mid, buf))  return 1;
    if (mergesort(v, mid, end, buf))    return 1;
    return merge(v, begin, mid, end, buf);
}

int test_mergesort(idx_t nmin, idx_t nmax, idx_t step, int n_iter, idx_t * const buf)
{
    idx_t * const v = malloc(nmax * sizeof(idx_t)); 
    if (v == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        return 1;
    }
    for(idx_t n=nmin; n<=nmax; n+=step)
    {
        clock_t begin, end, overall_time;
        overall_time = 0;
        for(int i=0; i<n_iter; ++i)
        {
            for(idx_t i=0; i<n;++i)
                v[i] = rand();

            begin = clock();
            int r = mergesort(v, 0, n, buf);
            end = clock();

            if (r != 0)
            {
                free(v);
                return 1;
            }
            
            // check for errors
            for(idx_t i=0; i<n-1; )
            {
                if (v[i] > v[++i])
                {
                    fprintf(stderr, "error in mergesort, size = %" PRId64 "\n", n);
                    free(v);
                    return 1;
                }
            }

            overall_time += (end - begin);
        }
        const double time = (double) overall_time/n_iter/CLOCKS_PER_SEC;
        printf("%" PRId64 " %f %d\n", n, time, n_iter);
    }
    free(v);
    return 0;
}

int test(idx_t nmin, idx_t nmax, idx_t step, int n_iter)
{
    idx_t * const buf = malloc(nmax/2 * sizeof(idx_t));
    if (buf == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        return 1;
    }
    int ret = test_mergesort(nmin, nmax, step, 1, buf);
    free(buf);
    return ret;
}

int main()
{
    static const idx_t sz = 430000;
    test(sz, sz, 1, 1);

    return 0;
}

