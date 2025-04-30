#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>

#define N 300  // Change this for different matrix sizes

int main() {
    float matrix1[N][N];
    float matrix2[N][N];
    float result[N][N];

    srand(0); // Fixed seed for reproducibility

    // Generate matrix1 and matrix2
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            matrix1[i][j] = rand() % 10;
            matrix2[i][j] = rand() % 10;
        }

    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    // Matrix multiplication
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < N; ++k)
                result[i][j] += matrix1[i][k] * matrix2[k][j];
        }

    // End timing
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Matrix multiplication took " << elapsed.count() << " seconds.\n";

    // Save matrix1
    std::ofstream out1("matrices/matrix1.txt");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            out1 << matrix1[i][j] << " ";
        out1 << "\n";
    }
    out1.close();

    // Save matrix2
    std::ofstream out2("matrices/matrix2.txt");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            out2 << matrix2[i][j] << " ";
        out2 << "\n";
    }
    out2.close();

    // Save result
    std::ofstream out3("matrices/result.txt");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            out3 << result[i][j] << " ";
        out3 << "\n";
    }
    out3.close();

    return 0;
}
