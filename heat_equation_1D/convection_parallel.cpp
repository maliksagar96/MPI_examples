#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mpi.h>
#include <sstream>
#include <vector>

void plot_with_gnuplot(const std::string &datafile, const std::string &imgfile,
                       double time) {
  FILE *gp = popen("gnuplot", "w");
  if (gp) {
    fprintf(gp,
            "set terminal pngcairo size 800,600\n"
            "set output '%s'\n"
            "set title '1D Linear Convection at t = %.3f'\n"
            "set xlabel 'x'\n"
            "set ylabel 'u'\n"
            "set yrange [0:2.2]\n"
            "plot '%s' using 1:2 with lines lw 2 title 'u(x)'\n"
            "exit\n",
            imgfile.c_str(), time, datafile.c_str());
    pclose(gp);
  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::filesystem::create_directory("images");

  const int nx_total = 10001;
  const double dx = 2.0 / (nx_total - 1);
  const double c = 1.0;
  const double dt = 0.01;
  const int nt = 50;

  // Domain decomposition
  int nx_local = nx_total / size;
  int start = rank * nx_local;
  int end = start + nx_local;

  std::vector<double> u(end - start, 1.0);
  std::vector<double> un(end - start);

  // Initial condition
  for (int i = 0; i < u.size(); ++i) {
    double x = (start + i) * dx;
    if (x >= 0.5 && x <= 1.0)
      u[i] = 2.0;
  }

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int n = 0; n <= nt; ++n) {
    // Gather u from all processes
    std::vector<double> u_global;
    if (rank == 0)
      u_global.resize(nx_total);
    MPI_Gather(u.data(), u.size(), MPI_DOUBLE, u_global.data(), u.size(),
               MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Only rank 0 saves and plots
    if (rank == 0) {
      std::ostringstream datafile, imgfile;
      datafile << "images/u_t" << std::setw(3) << std::setfill('0') << n
               << ".dat";
      imgfile << "images/u_t" << std::setw(3) << std::setfill('0') << n
              << ".png";

      std::ofstream fout(datafile.str());
      for (int i = 0; i < nx_total; ++i)
        fout << i * dx << " " << u_global[i] << "\n";
      fout.close();

      plot_with_gnuplot(datafile.str(), imgfile.str(), n * dt);
    }

    if (n == nt)
      break;

    // Exchange ghost values
    if (rank != 0)
      MPI_Sendrecv(&u[0], 1, MPI_DOUBLE, rank - 1, 0, &u[0], 1, MPI_DOUBLE,
                   rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (rank != size - 1)
      MPI_Sendrecv(&u.back(), 1, MPI_DOUBLE, rank + 1, 0, &u.back(), 1,
                   MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    un = u;
    for (int i = 1; i < u.size(); ++i) {
      u[i] = un[i] - c * dt / dx * (un[i] - un[i - 1]);
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  if (rank == 0) {
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Parallel simulation + plotting took " << elapsed.count()
              << " seconds.\n";
  }

  MPI_Finalize();
  return 0;
}
