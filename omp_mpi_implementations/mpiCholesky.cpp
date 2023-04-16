#include <iostream>
#include <cmath>
#include <chrono>
#include "mpi.h"

using namespace std;
using namespace std::chrono;

const int MAX_SIZE = 10;

void cholesky(double matrix[MAX_SIZE][MAX_SIZE], double L[MAX_SIZE][MAX_SIZE], int size, int rank, int num_procs) {
    int block_size = size / num_procs; // calculate the size of each block
    int start_row = rank * block_size; // calculate the starting row of the current block
    int end_row = (rank == num_procs - 1) ? size : (rank + 1) * block_size; // calculate the ending row of the current block

    for (int i = start_row; i < end_row; i++) {
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

int main(int argc, char** argv) {
    int size, rank, num_procs;
    double matrix[MAX_SIZE][MAX_SIZE], L[MAX_SIZE][MAX_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (rank == 0) {
        cout << "Enter the size of the matrix (maximum " << MAX_SIZE << "): ";
        cin >> size;

        cout << "Enter the matrix: " << endl;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cin >> matrix[i][j];
            }
        }
    }

    auto start = high_resolution_clock::now(); // start measuring time

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD); // broadcast the size of the matrix
    MPI_Bcast(&matrix, MAX_SIZE * MAX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD); // broadcast the matrix

    cholesky(matrix, L, size, rank, num_procs);

    MPI_Gather(&L, MAX_SIZE * MAX_SIZE / num_procs, MPI_DOUBLE, &L, MAX_SIZE * MAX_SIZE / num_procs, MPI_DOUBLE, 0, MPI_COMM_WORLD); // gather the L matrix

    auto stop = high_resolution_clock::now(); // stop measuring time
    auto duration = duration_cast<microseconds>(stop - start); // calculate the duration

    if (rank == 0) {
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
    }
    MPI_Finalize();

    cout << endl << "Execution time: " << duration.count() << " microseconds" << endl;

    return 0;
}