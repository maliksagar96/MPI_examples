#include <chrono>
#include <fstream>
#include <iostream>

using namespace std;

#define N 1024

int main(int argc, char *argv[]) {
  float matrix1[N][N + 1];
  // float matrix2[N][N+1];
  // float result[N][N+1];

  srand(0);
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N + 1; ++j) {
      matrix1[i][j] = rand() % 100;
      // matrix2[i][j] = rand() % 100;
    }

  // float matrix1[N][N+1] = {{1,3,6,25}, {2,7,14,58}, {0,2,5,19}};
  // float matrix1[N][N+1] = {{1,1,-1,-2}, {2,-1,1,5}, {-1,2,2,1}};
  // float matrix1[N][N + 1] = {{2, 1, 2, 10}, {1, 2, 1, 8}, {3, 1, -1, 2}};

  // Gauss Elimination
  for (int first_ptr = 0; first_ptr < N - 1; first_ptr++) {
    for (int second_ptr = first_ptr + 1; second_ptr < N; second_ptr++) {
      for (int column = N; column >= 0; column--) {
        matrix1[second_ptr][column] -=
            (matrix1[second_ptr][first_ptr] / matrix1[first_ptr][first_ptr]) *
            matrix1[first_ptr][column];
      }
    }
  }

  // Gauss Jordan
  for (int first_ptr = N - 1; first_ptr >= 0; first_ptr--) {
    for (int second_ptr = first_ptr - 1; second_ptr >= 0; second_ptr--) {
      for (int column = N; column >= 0; column--) {
        matrix1[second_ptr][column] -=
            (matrix1[second_ptr][first_ptr] / matrix1[first_ptr][first_ptr]) *
            matrix1[first_ptr][column];
      }
    }
  }

  for (int i = 0; i < N; i++) {
    matrix1[i][N] /= matrix1[i][i];
    matrix1[i][i] = 1;
  }

  // ofstream out1("matrices/matrix_1.txt");
  // for (int i = 0; i < N; ++i) {
  //   for (int j = 0; j < N + 1; ++j)
  //     out1 << matrix1[i][j] << " ";
  //   out1 << "\n";
  // }
  // out1.close();

  return 0;
}