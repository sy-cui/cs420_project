#include <iostream>
#include <fstream>
#include "utils.hpp"
#include "cholesky.hpp"

/* TODO: translate some of the params to command line arguments */
int main() {
    std::cout << "====== Running ... ========" << std::endl;
    int dim = 32;
    std::string in_fname = "../data/size" + std::to_string(dim) + ".txt";
    std::string out_fname = "../result.txt";

    // Generate a new matrix. This can be expensive
    generate_random_spd_matrix(in_fname, dim);

    SPDMatrix test_matrix(dim);
    test_matrix.load_from_file(in_fname);
    test_matrix.print_full_matrix();
    serial_cholesky(test_matrix, out_fname);
    return 0;   
}