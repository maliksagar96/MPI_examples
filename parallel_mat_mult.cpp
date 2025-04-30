#include <iostream>
#include <mpi.h>

#define N 2  // You can change this or make it variable

int main(int argc, char* argv[]) {
    int rank, size;
    float matrix1[N][N] = {{1, 2}, {3, 4}};
    float matrix2[N][N] = {{5, 6}, {7, 8}};
    float result[N][N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Each process will handle N / size rows (assuming N divisible by size)
    int rows_per_process = N / size;
    float local_matrix1[rows_per_process][N];
    float local_result[rows_per_process][N];

    // Scatter the rows of matrix1 to all processes
    MPI_Scatter(matrix1, rows_per_process * N, MPI_FLOAT,
                local_matrix1, rows_per_process * N, MPI_FLOAT,
                0, MPI_COMM_WORLD);

    // Broadcast matrix2 to all processes
    MPI_Bcast(matrix2, N * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Multiply local_matrix1 with matrix2
    for (int i = 0; i < rows_per_process; i++) {
        for (int j = 0; j < N; j++) {
            local_result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                local_result[i][j] += local_matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    // Gather the local results into the final result matrix on root
    MPI_Gather(local_result, rows_per_process * N, MPI_FLOAT,
               result, rows_per_process * N, MPI_FLOAT,
               0, MPI_COMM_WORLD);

    // Print result on root process
    if (rank == 0) {
        std::cout << "Result matrix:\n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                std::cout << result[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}
