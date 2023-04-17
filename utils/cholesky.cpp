#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "mpi.h"
#include "omp.h"
#include "cholesky.hpp"

void serial_cholesky(const SPDMatrix& A, std::string out_fname) {
    SPDMatrix L(A);
    for (int k = 0; k < L.dim; ++k) {
        L(k, k) = sqrt(L(k, k));

        for (int i = k + 1; i < L.dim; ++i) {
            L(i, k) /= L(k, k);
        }
        for (int j = k + 1; j < L.dim; ++j) {
            for (int i = j; i < L.dim; ++i) {
                L(i, j) -= L(i, k) * L(j, k);
            }
        }
    }
    L.write_to_file(out_fname);
}

void omp_cholesky(const SPDMatrix& A, std::string out_fname, int np) {
    SPDMatrix L(A);
    int i, j, k;

    omp_set_dynamic(0);
	omp_set_num_threads(np);

    for (k = 0; k < L.dim; ++k) {
        L(k, k) = sqrt(L(k, k));

        #pragma omp parallel for private(i) shared(k, L)
        for (i = k + 1; i < L.dim; ++i) {
            L(i, k) /= L(k, k);
        }

        #pragma omp parallel for private(i, j) shared(k, L)
        for (i = k + 1; i < L.dim; ++i) {
            for (j = k + 1; j < i + 1; ++j) {
                L(i, j) -= L(i, k) * L(j, k);
            }
        }
    }
    
    L.write_to_file(out_fname);
}

void mpi_cholesky(int rank, int size, int dim, std::map<int, std::vector<double>> &row_buffers) {
    // Figure out which rows the current process is in charge of
    auto curr_it = row_buffers.begin();
    std::vector<int> row_indices;
    for (int i = 0; i < dim; ++i) {
        if (i % size == rank) {
            row_indices.push_back(i);
        }
    }

    // Dynamic buffer for the current column
    double* curr_col = (double*)malloc(sizeof(double) * dim);

    // Main loop
    int curr_root_process;
    double pivot;

    for (int k = 0; k < dim; k++) {

        // A[k][k] = sqrt(A[k][k])
        curr_root_process = k % size;
        if (rank == curr_root_process) {
            pivot = sqrt(row_buffers[k][k]);
            row_buffers[k][k] = pivot;
            ++curr_it;   // Current row is fully factorized. 
        }

        // Broadcast sqrt(A[k][k]) to all processes
        MPI_Bcast(&pivot, 1, MPI_DOUBLE, curr_root_process, MPI_COMM_WORLD);

        // Scale the current column by the pivot on all processes
        // And store them in curr_col
        for (auto it = curr_it; it != row_buffers.end(); it++) {
            it->second[k] /= pivot;
            curr_col[it->first] = it->second[k];
        }

        // Each process broadcast the updated column values
        for (int i = k + 1; i < dim; i++) {
            MPI_Bcast(&curr_col[i], 1, MPI_DOUBLE, i % size, MPI_COMM_WORLD);
        }

        // Subtract the outer product
        for (auto it = curr_it; it != row_buffers.end(); it++) {
            for (int i = k + 1; i < it->first + 1; i++) {
                it->second[i] -= curr_col[it->first] * curr_col[i];
            }
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
    }

    free(curr_col);
}
