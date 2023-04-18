#ifndef CHOLESKY_HPP
#define CHOLESKY_HPP

#include "utils.hpp"
#include <vector>
#include <map>
#include <chrono>

double serial_cholesky(const SPDMatrix& A, std::string out_fname);
double omp_cholesky(const SPDMatrix& A, std::string out_fname, int np);
double mpi_cholesky(int rank, int size, int dim, std::map<int, std::vector<double>>&);

#endif  // CHOLESKY_HPP