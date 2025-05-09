#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>   // for popen, pclose
#include <cstdlib>  // for system()
#include <chrono>


using namespace std;

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();
    const int nx = 1e6;
    const int nt = 10000;      // Reduce nt for faster plotting
    double xStart = 0, xEnd = 2.0;
    double dx = (xEnd - xStart) / (nx - 1);
    double* u = new double[nx];
    double* un = new double[nx];
    double dt = 0.001;
    double c = 1.0;  // wave speed
    double pre_factor = c * dt / dx;

    // Create initial condition
    for (int i = 0; i < nx; i++) {
        u[i] = 0;
        if (i > nx / 4 && i < nx / 2)
            u[i] = 1.0;
    }

    // Create directories if not exist
    system("mkdir -p data");
    system("mkdir -p images");



    // Time loop
    for (int t = 0; t < nt; t++) {
        // Save data
        // string datafile = "data/u_" + to_string(t) + ".txt";
        // ofstream fout(datafile);
				
        // for (int i = 0; i < nx; i++) {
        //     double x = xStart + i * dx;
        //     fout << x << " " << u[i] << "\n";
        // }
        // fout.close();

        // Plot with gnuplot and save image
        // string imgfile = "images/u_" + to_string(t) + ".png";
        // FILE* gp = popen("gnuplot", "w");
        // if (gp) {
        //     fprintf(gp,
        //         "set terminal pngcairo size 800,600\n"
        //         "set output '%s'\n"
        //         "set title '1D Linear Convection at t = %d'\n"
        //         "set xlabel 'x'\n"
        //         "set ylabel 'u'\n"
        //         "set yrange [0:1.2]\n"
        //         "plot '%s' using 1:2 with lines notitle\n"
        //         "exit\n",
        //         imgfile.c_str(), t, datafile.c_str());
        //     pclose(gp);
        // }

        // Update solution (forward in time)
        for (int i = 1; i < nx; i++) {
          un[i] = u[i] - pre_factor * (u[i] - u[i - 1]);
        }

        un[0] = 0; // boundary condition

        for (int i = 0; i < nx; i++) {
            u[i] = un[i];
        }
    }

    delete[] u;
    delete[] un;

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    cout << "Execution time: " << elapsed.count() << " seconds\n";
    return 0;
}
