#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <inttypes.h>

#define MERGESORT_THRESHOLD 22

// use 64 bit indexes
typedef int64_t idx_t;
#define PRIidx_t PRId64
// sort chars
typedef char value_t;
#define PRIvalue_t "c"

void error_handler(const char* str)
{
    fprintf(stderr, "%s", str);
    exit(1);
}

int insertion_sort(value_t* v, int n)
{
    for(int i=1; i<n; ++i)
    {
        const value_t t = v[i];
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

int insertion_sort_count(value_t* v, int n, uint64_t* moves, uint64_t* comps)
{
    for(int i=1; i<n; ++i)
    {
        const value_t t = v[i];
        ++moves;
        int pos = i;
        while (pos > 0 && t < v[pos-1])
        {
            ++comps;
            v[pos] = v[pos-1];
            ++moves;
            --pos;
        }
        v[pos] = t;
        ++moves;
    }
    return 0;
}

int plain_sort(value_t* v, int n)
{
    return insertion_sort(v, n);
}

int plain_sort_count(value_t* v, int n, uint64_t* moves, uint64_t* comps)
{
    return insertion_sort_count(v, n, moves, comps);
}

int merge(value_t* v, idx_t begin, idx_t mid, idx_t end, value_t* buf)
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

int merge_count(value_t* v, idx_t begin, idx_t mid, idx_t end, value_t* buf, uint64_t* moves, uint64_t* comps)
{
    const idx_t n1 = mid - begin;
    for(idx_t i=0; i<n1; ++i)
        buf[i] = v[begin+i];
    *moves += n1;

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
        ++(*comps);
        ++(*moves);
    }

    while (i1 < n1)
    {
        v[begin+i1+i2] = buf[i1];
        ++(*moves);
        ++i1;
    }
    while (i2 < n2)
    {
        v[begin+i1+i2] = v[mid+i2];
        ++(*moves);
        ++i2;
    }
    return 0;
}

int mergesort(value_t* v, idx_t begin, idx_t end, value_t* buf)
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

int mergesort_count(value_t* v, idx_t begin, idx_t end, value_t* buf, uint64_t* moves, uint64_t* comps)
{
    // v is array indexed as [begin, end)
    const idx_t n = end - begin;
    if (n <= MERGESORT_THRESHOLD)
    {
        return plain_sort_count(v+begin, n, moves, comps);
    }

    const idx_t mid = (begin + end)/2;
    if (mergesort_count(v, begin, mid, buf, moves, comps))  return 1;
    if (mergesort_count(v, mid, end, buf, moves, comps))    return 1;
    return merge_count(v, begin, mid, end, buf, moves, comps);
}

int plain_mergesort(value_t* v, idx_t begin, idx_t end, value_t* buf)
{
    // v is array indexed as [begin, end)
    const idx_t n = end - begin;
    if (n==1)   return 0;

    const idx_t mid = (begin + end) / 2;
    if (plain_mergesort(v, begin, mid, buf))  return 1;
    if (plain_mergesort(v, mid, end, buf))    return 1;
    return merge(v, begin, mid, end, buf);
}

// counts moves and comparisons
int plain_mergesort_count(value_t* v, idx_t begin, idx_t end, value_t* buf, uint64_t* moves, uint64_t* comps)
{
    // v is array indexed as [begin, end)
    const idx_t n = end - begin;
    if (n==1)   return 0;

    const idx_t mid = (begin + end) / 2;
    if (plain_mergesort_count(v, begin, mid, buf, moves, comps))  return 1;
    if (plain_mergesort_count(v, mid, end, buf, moves, comps))    return 1;
    return merge_count(v, begin, mid, end, buf, moves, comps);
}

int time_sort(int (*sort)(value_t*, idx_t, idx_t, value_t*), idx_t nmin, idx_t nmax, idx_t step, int n_iter, value_t* buf, FILE* f)
{
    value_t* const v = malloc(nmax * sizeof(value_t)); 
    if (v == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        return 1;
    }
    for(idx_t n=nmin; n<=nmax; n*=step)
    {
        long long overall_time = 0;
        for(int i=0; i<n_iter; ++i)
        {
            for(idx_t i=0; i<n;++i)
                v[i] = rand();

            long long begin = PAPI_get_virt_usec();
            int r = sort(v, 0, n, buf);
            long long end = PAPI_get_virt_usec();

            if (r != 0)
            {
                free(v);
                return 1;
            }
            
            // check for errors
            for(idx_t i=0; i<n-1;++i)
            {
                if (v[i] > v[i+1])
                {
                    fprintf(stderr, "error in mergesort, size = %" PRId64 "\n", n);
                    free(v);
                    return 1;
                }
            }

            overall_time += (end-begin);
        }
        const double time = (double) overall_time/n_iter/1e6;
        fprintf(f, "%" PRId64 " %f %d\n", n, time, n_iter);
    }
    free(v);
    return 0;
}

int count_sort(int (*sort)(value_t*, idx_t, idx_t, value_t*, uint64_t*, uint64_t*), idx_t nmin, idx_t nmax, idx_t step, int n_iter, value_t* buf, FILE* f)
{
    value_t* const v = malloc(nmax * sizeof(value_t)); 
    if (v == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        return 1;
    }
    for(idx_t n=nmin; n<=nmax; n*=step)
    {
        uint64_t moves = 0;
        uint64_t comps = 0;
        for(int i=0; i<n_iter; ++i)
        {
            for(idx_t i=0; i<n;++i)
                v[i] = rand();

            if (sort(v, 0, n, buf, &moves, &comps))
            {
                free(v);
                return 1;
            }
            
            // check for errors
            for(idx_t i=0; i<n-1;++i)
            {
                if (v[i] > v[i+1])
                {
                    fprintf(stderr, "error in mergesort, size = %" PRId64 "\n", n);
                    free(v);
                    return 1;
                }
            }
        }
        fprintf(f, "%" PRIidx_t " %" PRIu64 " %" PRIu64 " %d\n", n, moves/n_iter, comps/n_iter, n_iter);
    }
    free(v);
    return 0;
}

int test(idx_t nmin, idx_t nmax, idx_t step, int n_iter)
{
    value_t* const buf = malloc(nmax/2 * sizeof(value_t));
    if (buf == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        return 1;
    }

    /* run all tests here */

    // time mergesort and plain_mergesort
    int retval;
    FILE* f1 = fopen("res/time_plain_mergesort.txt", "w");
    retval = time_sort(&plain_mergesort, nmin, nmax, step, n_iter, buf, f1);
    fclose(f1);
    if (retval)
    {
        free(buf);
        return 1;
    }
    // tracking progress
    printf("25%%\n");

    FILE* f2 = fopen("res/time_mergesort.txt", "w");
    retval = time_sort(&mergesort, nmin, nmax, step, n_iter, buf, f2);
    fclose(f2);
    if (retval)
    {
        free(buf);
        return 1;
    }
    printf("50%%\n");

    // moves, comparisons
    f1 = fopen("res/count_mergesort.txt", "w");
    retval = count_sort(&mergesort_count, nmin, nmax, step, n_iter, buf, f1);
    fclose(f1);
    if (retval)
    {
        free(buf);
        return 1;
    }
    printf("75%%\n");

    f2 = fopen("res/count_plain_mergesort.txt", "w");
    retval = count_sort(&plain_mergesort_count, nmin, nmax, step, n_iter, buf, f2);
    fclose(f2);
    if (retval)
    {
        free(buf);
        return 1;
    }
    printf("100%%\n");

    free(buf);
    return 0;
}

int main()
{
    static const idx_t sz1 = 1e2;
    static const idx_t sz2 = 4.3e9;
    static const idx_t step = 2;
    test(sz1, sz2, step, 1);

    return 0;
}

