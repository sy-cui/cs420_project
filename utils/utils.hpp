#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

struct SPDMatrix 
{
    double* data;
    unsigned int dim;
    unsigned int size;

    SPDMatrix(const int& dim);
    ~SPDMatrix();
    SPDMatrix(const SPDMatrix& other);

    double& operator()(unsigned int row, unsigned int column);
    void write_to_file(std::string) const;
    void load_from_file(std::string);
    void print_full_matrix();
};

void generate_random_spd_matrix(std::string fname, int dim);
#endif // UTILS_HPP