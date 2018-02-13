#include <stdio.h>
#include <mpi.h>
#include <math.h>

void barrier()
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int depth = 1;
    int max_depth = (int) ceil(log2(size));

    while (depth <= max_depth)
    {
        int gap = (int) pow(2, depth-1);

        if (rank % gap == 0)
        {
            if (rank % (2*gap) > 0)
            {
                int x = 2;
                int to_rank = rank - gap;
                MPI_Send(&x, 1, MPI_INT, to_rank, 0, MPI_COMM_WORLD);
            }
            else
            {
                int x;
                int from_rank = rank + gap;
                if (from_rank < size)
                {
                    MPI_Recv(&x, 1, MPI_INT, from_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                } 
            }
        }
        ++depth;
    }

    depth = max_depth;

    while (depth >= 1)
    {
        int gap = (int) pow(2, depth-1);

        if (rank % gap == 0)
        {
            if (rank % (2*gap) == 0)
            {
                int x = 2;
                int to_rank = rank + gap;
                if (to_rank < size)
                {
                    MPI_Send(&x, 1, MPI_INT, to_rank, 0, MPI_COMM_WORLD);
                }
            }
            else
            {
                int x;
                int from_rank = rank - gap;
                MPI_Recv(&x, 1, MPI_INT, from_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        --depth;
    }
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("%d before\n", rank);
    barrier();
    barrier();
    printf("%d after\n",  rank);

    MPI_Finalize();

    return 0;
}
