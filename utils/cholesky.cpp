#include "cholesky.hpp"
#include <cmath>
#include <iostream>

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