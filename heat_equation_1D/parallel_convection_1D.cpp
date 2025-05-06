#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>   // for popen, pclose
#include <cstdlib>  // for system()
#include <mpi.h>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int nx = 1000;
    const int nt = 1000;
    double xStart = 0, xEnd = 2.0;
    double dx = (xEnd - xStart) / (nx - 1);
    double dt = 0.001;
    double c = 1.0;
    double pre_factor = c * dt / dx;

    int local_nx = nx / size;
    int start = rank * local_nx;
    int end = (rank == size - 1) ? nx : start + local_nx;

    double* u = new double[nx];
    double* un = new double[nx];

    // Initial condition (only rank 0 needs to set it)
    if (rank == 0) {
        for (int i = 0; i < nx; i++) {
            u[i] = 0;
            if (i > nx / 4 && i < nx / 2)
                u[i] = 1.0;
        }
    }

    MPI_Bcast(u, nx, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        system("mkdir -p data");
        system("mkdir -p images");
    }

    for (int t = 0; t < nt; t++) {
        // Exchange ghost values
        if (rank != 0)
            MPI_Send(&u[start], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
        if (rank != size - 1)
            MPI_Recv(&u[end], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (rank != size - 1)
            MPI_Send(&u[end - 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
        if (rank != 0)
            MPI_Recv(&u[start - 1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Update local region
        for (int i = start + 1; i < end; i++) {
            un[i] = u[i] - pre_factor * (u[i] - u[i - 1]);
        }

        // Boundary condition
        if (rank == 0)
            un[0] = 0;

        MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, un, local_nx, MPI_DOUBLE, MPI_COMM_WORLD);

        for (int i = start; i < end; i++)
            u[i] = un[i];

        // Save file and plot (only rank 0)
        if (rank == 0) {
            string datafile = "data/u_" + to_string(t) + ".txt";
            ofstream fout(datafile);
            for (int i = 0; i < nx; i++) {
                double x = xStart + i * dx;
                fout << x << " " << u[i] << "\n";
            }
            fout.close();

            string imgfile = "images/u_" + to_string(t) + ".png";
            FILE* gp = popen("gnuplot", "w");
            if (gp) {
                fprintf(gp,
                    "set terminal pngcairo size 800,600\n"
                    "set output '%s'\n"
                    "set title '1D Linear Convection at t = %d'\n"
                    "set xlabel 'x'\n"
                    "set ylabel 'u'\n"
                    "set yrange [0:1.2]\n"
                    "plot '%s' using 1:2 with lines notitle\n"
                    "exit\n",
                    imgfile.c_str(), t, datafile.c_str());
                pclose(gp);
            }
        }
    }

    delete[] u;
    delete[] un;
    MPI_Finalize();
    return 0;
}
