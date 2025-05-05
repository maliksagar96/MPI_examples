#include <iostream>
#include <fstream>
#include <mpi.h>

using namespace std;

#define N 1024

int main(int argc, char* argv[]) {
  float matrix1[N][N+1];
  // float matrix1[N][N+1] = {{1,1,2,2,3,3,5}, {12,13,1,2,3,4,5}, {15,21,12,11,156,14,167}, {12,12,145,223,18,98,12}, {67,89,57,30,20,69,88}, {45, 78, 78, 54, 76, 99, 99}};
  // float matrix1[N][N+1] = {{1,1,2,2,3,3,51, 12, 21}, {12,13,1,2,3,4,42, 156, 556}, {15,21,12,11,156,14,167,112,212}, {12,12,145,223,18,98,12,23,345}, {67,89,57,30,20,69,88,465,234}, {45, 78, 78, 54, 76, 99, 99,2,123} , {2,5,8,123,11,14,19,12,678}, {1,85,14,67,49,94,49,78,1}};
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Initialize matrix only in rank 0
  if (rank == 0) {
      srand(0);
      for (int i = 0; i < N; ++i)
          for (int j = 0; j < N+1; ++j)
              matrix1[i][j] = rand() % 100;
      
  }

  int rows_scatter = N / size;
  float local_matrix[rows_scatter][N+1];

  double start_time = MPI_Wtime();

  for (int pivot_ptr = 0; pivot_ptr < N - 1; pivot_ptr++) {
      MPI_Scatter(matrix1, (N + 1) * rows_scatter, MPI_FLOAT,
                  local_matrix, (N + 1) * rows_scatter, MPI_FLOAT,
                  0, MPI_COMM_WORLD);

      float pivot_row[N + 1];
      if (rank == 0) {
        for (int i = 0; i < N + 1; ++i)
          pivot_row[i] = matrix1[pivot_ptr][i];
      }

      MPI_Bcast(pivot_row, N + 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

      for (int local_row = 0; local_row < rows_scatter; local_row++) {
        int global_row = rank * rows_scatter + local_row;
        if (global_row > pivot_ptr) {
          float factor = local_matrix[local_row][pivot_ptr] / pivot_row[pivot_ptr];
          for (int col = 0; col < N + 1; col++)
            local_matrix[local_row][col] -= factor * pivot_row[col];
          }
      }

      MPI_Gather(local_matrix, (N + 1) * rows_scatter, MPI_FLOAT,
                  matrix1, (N + 1) * rows_scatter, MPI_FLOAT,
                  0, MPI_COMM_WORLD);
  }

    double end_time = MPI_Wtime();

  //Gauss Jordan 
  for(int pivot_ptr = N-1;pivot_ptr>=0;pivot_ptr--) {
    
    MPI_Scatter(matrix1, (N + 1) * rows_scatter, MPI_FLOAT,
                  local_matrix, (N + 1) * rows_scatter, MPI_FLOAT,
                  0, MPI_COMM_WORLD);

    float pivot_row[N + 1];
    if (rank == 0) {
      for (int i = 0; i < N + 1; ++i)
        pivot_row[i] = matrix1[pivot_ptr][i];
    }

    MPI_Bcast(pivot_row, N + 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // for (int local_row = 0; local_row < rows_scatter; local_row++) {
    //   int global_row = rank * rows_scatter + local_row;
    //   if (global_row > pivot_ptr) {
    //     float factor = local_matrix[local_row][pivot_ptr] / pivot_row[pivot_ptr];
    //     for (int col = 0; col < N + 1; col++)
    //       local_matrix[local_row][col] -= factor * pivot_row[col];
    //     }
    // }


    for(int local_row = 0; local_row < rows_scatter; local_row++) {
      int global_row = rank * rows_scatter + local_row;
        if (global_row != pivot_ptr) {
          float factor = local_matrix[local_row][pivot_ptr] / pivot_row[pivot_ptr];
          for (int col = 0; col < N + 1; col++) {
            local_matrix[local_row][col] -= factor * pivot_row[col];
          }              
        }    
    }

    MPI_Gather(local_matrix, (N + 1) * rows_scatter, MPI_FLOAT,
              matrix1, (N + 1) * rows_scatter, MPI_FLOAT,
              0, MPI_COMM_WORLD);
    
  }

  if (rank == 0) {

    for(int i = 0;i<N;i++) {
      matrix1[i][N] /= matrix1[i][i];
      matrix1[i][i] = 1;
    }

      ofstream out1("matrices/matrix_parallel_1.txt");
      for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N + 1; ++j) {
          if(abs(matrix1[i][j]) < 1e-4) {
            matrix1[i][j] = 0;
          }            
          out1 << matrix1[i][j] << " ";    
        } 
        out1 << "\n";           
      }
      out1.close();

      cout << "Elapsed time: " << end_time - start_time << " seconds.\n";
  }

  MPI_Finalize();
  return 0;
}
