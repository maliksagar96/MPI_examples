#include <iostream> 
#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {
  // Start timing
  auto start = chrono::high_resolution_clock::now();
  const int nx = 1000000;
  float xi, xf, dx;
  xi = 0;
  xf = 1000;

  dx = (xf-xi)/(nx-1);

  float fx[nx];

  for(int i =0;i<nx;i++) {  
    fx[i] = (i*dx)*(i*dx);    
  }

  float area = 0;

  for(int i = 0;i<nx-1;i++) {
    area+= (dx) * (fx[i] + fx[i+1])/2;
  }

  cout<<"Area = "<<area<<endl;

  auto end = chrono::high_resolution_clock::now();
  chrono::duration<double> elapsed = end - start;
  cout << "Trapezoidal method took " << elapsed.count() << " seconds.\n";
  return 0;
}