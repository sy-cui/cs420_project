#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "mpi.h"
#include "utils.hpp"
#include "cholesky.hpp"

int run_serial(int dim, bool gen_new) {
    std::string in_fname = "../data/size" + std::to_string(dim) + ".txt";
    std::string out_fname = "../result/serial_dim_" + std::to_string(dim) + ".txt";

    // Generate a new matrix. This can be expensive
    if (gen_new) {
        generate_random_spd_matrix(in_fname, dim);
    }

    SPDMatrix test_matrix(dim);
    test_matrix.load_from_file(in_fname);
    // test_matrix.print_full_matrix();
    serial_cholesky(test_matrix, out_fname);
    return 0;
}

int run_mpi(int dim, bool gen_new) {
    int tril_size = dim * (dim + 1) / 2;

    std::string in_fname = "../data/size" + std::to_string(dim) + ".txt";
    
    // Generate a new matrix. This can be expensive
    if (gen_new) {
        generate_random_spd_matrix(in_fname, dim);
    }

    // Initialize MPI communicator
    MPI_Init(NULL, NULL);

    int rank, size;

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

        std::string out_fname = (
            "../result/mpi_np_" + std::to_string(size) 
            + "_dim_" + std::to_string(dim) 
            + ".txt"
        );
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
    int dim = 4;
    bool gen_new = false;
    int run_option = 0;
    int np = 1;

    for (int i = 0; i < argc; i++) {
        std::string s(argv[i]);
        if (s == "-d" || s == "--dim") {
            dim = std::stoi(argv[i + 1]);
        }

        if (s == "-g" || s == "--gen-new") {
            gen_new = true;
        }

        if (s == "-s" || s == "--serial") {
            run_option = 0;
        }

        if (s == "-o" || s == "--omp") {
            run_option = 1;
        }

        if (s == "-m" || s == "--mpi") {
            run_option = 2;
        }

        if (s == "-p" || s == "--num-process") {
            np = std::stoi(argv[i + 1]);
        }
    }

    switch (run_option) {
        case 1:
            std::cout << "==== Running OpenMP Cholesky ====\n";
            std::cout << "Dimensions = " << dim << "\n";
            std::cout << "Number of processes = " << np << "\n";
            if (gen_new) {
                std::cout << "Generating new matrix ...\n";
            }
            std::cout << "=================================\n";
            // return run_omp(dim, gen_new, np);
            break;
        case 2:
            std::cout << "==== Running MPI Cholesky ====\n";
            std::cout << "Dimensions = " << dim << "\n";
            if (gen_new) {
                std::cout << "Generating new matrix ...\n";
            }
            std::cout << "==============================\n";
            return run_mpi(dim, gen_new);
        default:
            std::cout << "==== Running Serial Cholesky ====\n";
            std::cout << "Dimensions = " << dim << "\n";
            if (gen_new) {
                std::cout << "Generating new matrix ...\n";
            }
            std::cout << "=================================\n";
            return run_serial(dim, gen_new);
    }
}