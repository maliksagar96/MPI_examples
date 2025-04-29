#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]) {
    // Define a 2x2 matrix
    int matrix1[2][2] = {{1, 2}, {3, 4}};  // Initialize matrix on rank 0

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Broadcast the matrix from rank 0 to all other ranks
    MPI_Bcast(&matrix1, 4, MPI_INT, 0, MPI_COMM_WORLD);

    // Synchronize all processes to reach the print point
    MPI_Barrier(MPI_COMM_WORLD);

    // Each process prints the matrix it received
    std::cout << "Rank " << rank << " received matrix:\n";
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            std::cout << matrix1[i][j] << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}
