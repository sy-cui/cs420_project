#include <random>
#include <fstream>
#include <iostream>
#include "utils.hpp"

SPDMatrix::SPDMatrix(const int& dim): 
    dim(dim), size(dim * (dim + 1) / 2) {
        data = (double*)malloc(sizeof(double) * size);
    }

SPDMatrix::~SPDMatrix() {
    free(data);
}

SPDMatrix::SPDMatrix(const SPDMatrix& other) {
    dim = other.dim;
    size = other.size;
    data = (double*)malloc(sizeof(double) * size);
    for (int i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
    
}

double& SPDMatrix::operator()(unsigned int row, unsigned int column) {
    if (row >= column) {
        return data[row * (row + 1) / 2 + column];
    } else {
        return data[column * (column + 1) / 2 + row];
    }
    
}

void SPDMatrix::write_to_file(std::string fname) const {
    std::ofstream outfile(fname);
    for (int i = 0; i < size; ++i) {
        outfile << data[i];
        outfile << "\n";
    }
}

void SPDMatrix::load_from_file(std::string fname) {
    std::ifstream infile;
    infile.open(fname);
    if (infile.is_open()){
        for (int i = 0; i < size; ++i) {
            infile >> data[i];
        }
    } else {
        std::cout << "Error opening file." << std::endl;
    }
    infile.close();
}

void SPDMatrix::print_full_matrix() {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            std::cout << (*this)(i, j) << "\t";
        }
        std::cout << "\n";
    }
}


void generate_random_spd_matrix(std::string fname, int dim) {
    // https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // Generate random matrix A
    double* random_matrix = (double*)malloc(sizeof(double) * dim * dim);
    double* spd_full_matrix = (double*)malloc(sizeof(double) * dim * dim);
    // double random_matrix[dim][dim], spd_full_matrix[dim][dim];
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            random_matrix[i * dim + j] = dist(gen);
        }
    }

    // A * transpose(A)
    double scale_factor;
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            spd_full_matrix[i * dim + j] = 0.0;
            for (int k = 0; k < dim; ++k) {
                spd_full_matrix[i * dim + j] += random_matrix[i * dim + k] * random_matrix[j * dim + k];
            }
            if (i == 0 && j == 0) {
                scale_factor = spd_full_matrix[0];
            }
            spd_full_matrix[i * dim + j] /= scale_factor;
        }
    }

    // Write to file
    std::ofstream outfile(fname);
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j <= i; ++j) {
            outfile << std::setprecision(std::numeric_limits<double>::digits10 + 2) << spd_full_matrix[i * dim + j];
            outfile << "\n";
        }
    }
    free(random_matrix);
    free(spd_full_matrix);
}