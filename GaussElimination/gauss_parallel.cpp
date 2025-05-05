#include <iostream>
#include <mpi.h>

using namespace std;

#define N 8

int main(int argc, char* argv[]) {
    
  float A[N][N+1];
  
  srand(0); 
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N+1; ++j) {
        A[i][j] = rand() % 100;
  
    }

    int rank, size;
    // float A[N][N+1] = {
    //     {2, 1, 2, 10},
    //     {1, 2, 1, 8},
    //     {3, 1, -1, 2}
    // };

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_proc = N / size;
    float local_rows[rows_per_proc][N+1];

    MPI_Scatter(A, rows_per_proc * (N+1), MPI_FLOAT,
                local_rows, rows_per_proc * (N+1), MPI_FLOAT,
                0, MPI_COMM_WORLD);

    for (int pivot = 0; pivot < N; pivot++) {
        float pivot_row[N+1];

        if ((pivot / rows_per_proc) == rank) {
            int local_index = pivot % rows_per_proc;
            for (int j = 0; j < N+1; j++)
                pivot_row[j] = local_rows[local_index][j];
        }

        MPI_Bcast(pivot_row, N+1, MPI_FLOAT, pivot / rows_per_proc, MPI_COMM_WORLD);

        for (int i = 0; i < rows_per_proc; i++) {
            int global_row = rank * rows_per_proc + i;
            if (global_row > pivot) {
                float factor = local_rows[i][pivot] / pivot_row[pivot];
                for (int j = 0; j < N+1; j++) {
                    local_rows[i][j] -= factor * pivot_row[j];
                }
            }
        }
    }

    MPI_Gather(local_rows, rows_per_proc * (N+1), MPI_FLOAT,
               A, rows_per_proc * (N+1), MPI_FLOAT,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Upper triangular matrix:\n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N+1; j++)
                cout << A[i][j] << " ";
            cout << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
