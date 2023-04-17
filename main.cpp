#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "mpi.h"
#include "utils.hpp"
#include "cholesky.hpp"

int run_serial(int argc, char* argv[]) {
    int dim = 32;
    std::string in_fname = "../data/size" + std::to_string(dim) + ".txt";
    std::string out_fname = "../result.txt";

    // Generate a new matrix. This can be expensive
    // generate_random_spd_matrix(in_fname, dim);

    SPDMatrix test_matrix(dim);
    test_matrix.load_from_file(in_fname);
    // test_matrix.print_full_matrix();
    serial_cholesky(test_matrix, out_fname);
    return 0;   
}

int run_mpi(int argc, char* argv[]) {
    int dim = 32;
    int tril_size = dim * (dim + 1) / 2;
    // generate_random_spd_matrix("../data/size" + std::to_string(dim) + ".txt", dim);
    MPI_Init(nullptr, nullptr);

    int rank, size;
    std::string in_fname = "../data/size" + std::to_string(dim) + ".txt";
    std::string out_fname = "../result_mpi.txt";

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::map<int, std::vector<double>> row_buffers;

    // Distribute matrix from p0 cyclically to all ranks
    if (rank == 0) {
        int dest, tag = 0;
        SPDMatrix original_matrix(dim);
        original_matrix.load_from_file(in_fname);

        for (int i = 0; i < dim; ++i) {
            dest = i % size;
            if (dest == rank) {
                std::vector<double> buffer(i + 1);
                for (int j = 0; j < i + 1; ++j) {
                    buffer[j] = original_matrix(i, j);
                }
                row_buffers[i] = buffer;

            } else {
                MPI_Send(&original_matrix(i, 0), i + 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
            }
            
            if (i / size != tag) {
                tag++;    // Increment tag after sending to each process once
            }
        }

    } else {
        int tag = 0;
        for (int i = 0; i < dim; ++i) {
            if (i % size == rank) {
                std::vector<double> buffer(i + 1);
                MPI_Recv(&buffer[0], i + 1, MPI_DOUBLE, 0, tag++, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                row_buffers[i] = buffer;
            }
        }
    }

    // Cholesky factorization 
    // After this call all row_buffers should be correctly factorized
    mpi_cholesky(rank, size, dim, row_buffers);

    // // All processes send the rows back to process 0
    MPI_Request reqs[dim];
    int num_reqs = 0, row_start;

    if (rank == 0) {
        double* soln = (double*)malloc(sizeof(double) * tril_size);
        for (int i = 0; i < dim; i++) {
            row_start = i * (i + 1) / 2;
            if (i % size == rank) {
                for (int j = 0; j < i + 1; j++) {
                    soln[row_start + j] = row_buffers[i][j];
                }

            } else {
                MPI_Irecv(&soln[row_start], i + 1, MPI_DOUBLE, 
                    i % size, i, MPI_COMM_WORLD, &reqs[num_reqs++]
                );  // tag is the row index
            }
        }
        MPI_Waitall(num_reqs, reqs, MPI_STATUS_IGNORE);

        std::ofstream outfile(out_fname);
        for (int i = 0; i < tril_size; ++i) {
            outfile << soln[i];
            outfile << "\n";
        }

        free(soln);

    } else {
        for (auto &row : row_buffers) {
            MPI_Isend(&row.second[0], row.first + 1, MPI_DOUBLE,
                0, row.first, MPI_COMM_WORLD, &reqs[num_reqs++]
            );
        }
        MPI_Waitall(num_reqs, reqs, MPI_STATUS_IGNORE);
    }

    MPI_Finalize();

    return 0;
}

int main(int argc, char* argv[]) {
    return run_mpi(argc, argv);
}