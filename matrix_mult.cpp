#include <iostream> 

using namespace std;

#define N 2

int main() {


	float matrix1[N][N] = {{1,2}, {3, 4}};
	float matrix2[N][N] = {{5,6}, {7, 8}};


  float result[N][N];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			result[i][j] = 0;
			for (int k = 0; k < N; k++) {
				result[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}


	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
				cout << result[i][j] << " ";
		}
		cout << "\n";
}
	
	return 0;
}