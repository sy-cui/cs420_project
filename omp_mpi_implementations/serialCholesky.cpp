#include <iostream>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int MAX_SIZE = 10;

void cholesky(double matrix[MAX_SIZE][MAX_SIZE], double L[MAX_SIZE][MAX_SIZE], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0;
            if (j == i) {
                for (int k = 0; k < j; k++) {
                    sum += pow(L[j][k], 2);
                }
                L[j][j] = sqrt(matrix[j][j] - sum);
            }
            else {
                for (int k = 0; k < j; k++) {
                    sum += (L[i][k] * L[j][k]);
                }
                L[i][j] = (matrix[i][j] - sum) / L[j][j];
            }
        }
    }
}

int main() {
    int size;
    double matrix[MAX_SIZE][MAX_SIZE], L[MAX_SIZE][MAX_SIZE];

    cout << "Enter the size of the matrix (maximum " << MAX_SIZE << "): ";
    cin >> size;

    cout << "Enter the matrix: " << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cin >> matrix[i][j];
        }
    }

    auto start = high_resolution_clock::now(); // start measuring time

    cholesky(matrix, L, size);

    auto stop = high_resolution_clock::now(); // stop measuring time
    auto duration = duration_cast<microseconds>(stop - start); // calculate the duration

    cout << endl << "Lower Triangle Matrix (L): " << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (j > i) {
                cout << "0\t";
            }
            else {
                cout << L[i][j] << "\t";
            }
        }
        cout << endl;
    }

    cout << endl << "Transpose Matrix of L: " << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i > j) {
                cout << "0\t";
            }
            else {
                cout << L[j][i] << "\t";
            }
        }
        cout << endl;
    }

    cout << endl << "Execution time: " << duration.count() << " microseconds" << endl;

    return 0;
}
