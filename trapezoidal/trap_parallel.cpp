#include <iostream>
#include <mpi.h>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    chrono::high_resolution_clock::time_point start, end;

    if (rank == 0) {
        start = chrono::high_resolution_clock::now();
    }

    if(rank == 0) {      
      auto start = chrono::high_resolution_clock::now();
    }

    const int nx = 1000000;
    float xi, xf;
    xi = 0;
    xf = 1000;
    
    float dx = (xf - xi) / (nx - 1);
    float fx[nx];

    if (rank == 0) {
        for (int i = 0; i < nx; i++) {
            float x = xi + i * dx;
            fx[i] = x * x;
        }
    }

    int rows_per_process = nx / size;
    float local_fx[rows_per_process];

    MPI_Scatter(fx, rows_per_process, MPI_FLOAT,
                local_fx, rows_per_process, MPI_FLOAT,
                0, MPI_COMM_WORLD);


    float local_area = 0.0;
    for (int i = 0; i < rows_per_process; i++) {
        local_area += dx * (local_fx[i] + local_fx[i + 1]) / 2.0;
    }

    // End timing
    

    float total_areas[size]; 
    MPI_Gather(&local_area, 1, MPI_FLOAT,
               total_areas, 1, MPI_FLOAT,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
      end = chrono::high_resolution_clock::now();
      chrono::duration<double> elapsed = end - start;
      cout << "Trapezoidal method took " << elapsed.count() << " seconds.\n";
        float sum_area = 0.0;
        for (int i = 0; i < size; i++) {
            sum_area += total_areas[i];
        }
        cout << "Estimated Area = " << sum_area << endl;
    }

    MPI_Finalize();
    return 0;
}
