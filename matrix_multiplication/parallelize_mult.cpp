#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mpi.h>

using namespace std;

#define N 300 // Change this for different matrix sizes

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (N % size != 0) {
    if (rank == 0)
      cout << "N must be divisible by number of processes.\n";
    MPI_Finalize();
    return 1;
  }

  float matrix1[N][N];
  float matrix2[N][N];
  float result[N][N];

  int rows_per_process = N / size;
  float local_matrix1[rows_per_process][N];
  float local_result[rows_per_process][N];

  // Only rank 0 generates the full matrices
  if (rank == 0) {
    srand(0); // fixed seed for reproducibility
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j) {
        matrix1[i][j] = rand() % 10;
        matrix2[i][j] = rand() % 10;
      }
  }

  // Scatter rows of matrix1 to all processes
  MPI_Scatter(matrix1, rows_per_process * N, MPI_FLOAT, local_matrix1,
              rows_per_process * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

  // Broadcast matrix2 to all processes
  MPI_Bcast(matrix2, N * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

  // Start timing
  // auto start = chrono::high_resolution_clock::now();

  // Matrix multiplication
  for (int i = 0; i < rows_per_process; ++i)
    for (int j = 0; j < N; ++j) {
      local_result[i][j] = 0;
      for (int k = 0; k < N; ++k)
        local_result[i][j] += local_matrix1[i][k] * matrix2[k][j];
    }

  // // End timing
  // auto end = chrono::high_resolution_clock::now();
  // chrono::duration<double> elapsed = end - start;

  // Gather results back to rank 0
  MPI_Gather(local_result, rows_per_process * N, MPI_FLOAT, result,
             rows_per_process * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    // cout << "Matrix multiplication took " << elapsed.count() << "
    // seconds.\n";

    // Save matrix1
    ofstream out1("matrices/matrix_parallel_1.txt");
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j)
        out1 << matrix1[i][j] << " ";
      out1 << "\n";
    }
    out1.close();

    // Save matrix2
    ofstream out2("matrices/matrix_parallel_2.txt");
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j)
        out2 << matrix2[i][j] << " ";
      out2 << "\n";
    }
    out2.close();

    // Save result
    ofstream out3("matrices/result_parallel.txt");
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j)
        out3 << result[i][j] << " ";
      out3 << "\n";
    }
    out3.close();
  }

  MPI_Finalize();
  return 0;
}
