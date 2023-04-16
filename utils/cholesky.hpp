#ifndef CHOLESKY_HPP
#define CHOLESKY_HPP

#include "utils.hpp"

void serial_cholesky(const SPDMatrix& A, std::string out_fname);
void omp_cholesky(const SPDMatrix& A, std::string out_fname);
void mpi_cholesky(const SPDMatrix& A, std::string out_fname);

#endif  // CHOLESKY_HPP