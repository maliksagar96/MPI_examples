#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

int main() {
  std::filesystem::create_directory("images_serial");

  const int nx = 10001;
  const double dx = 2.0 / (nx - 1);
  const double c = 1.0;
  const double dt = 0.001;
  const int nt = 100;
  std::vector<double> u(nx, 1.0);

  // Initial condition
  for (int i = 0; i < nx; ++i) {
    double x = i * dx;
    if (x >= 0.5 && x <= 1.0)
      u[i] = 2.0;
  }

  // Start timing
  auto start = std::chrono::high_resolution_clock::now();

  for (int n = 0; n <= nt; ++n) {
    // Write data to file
    std::ostringstream datafile;
    datafile << "images_serial/u_t" << std::setw(3) << std::setfill('0') << n
             << ".dat";
    std::ofstream fout(datafile.str());
    for (int i = 0; i < nx; ++i) {
      double x = i * dx;
      fout << x << " " << u[i] << "\n";
    }
    fout.close();

    // Plot using gnuplot
    std::ostringstream imgfile;
    imgfile << "images_serial/u_t" << std::setw(3) << std::setfill('0') << n
            << ".png";
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
              imgfile.str().c_str(), n * dt, datafile.str().c_str());
      pclose(gp);
    }

    if (n == nt)
      break;

    // Update solution
    std::vector<double> un = u;
    for (int i = 1; i < nx; ++i) {
      u[i] = un[i] - c * dt / dx * (un[i] - un[i - 1]);
    }
  }

  // End timing
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  std::cout << "Simulation + plotting took " << duration.count()
            << " seconds.\n";

  return 0;
}
