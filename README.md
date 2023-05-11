# CS 420 Final Project
Final project for CS 420: OpenMP and MPI implementation of Cholesky factorization

# To compile the code 
```bash
bash build_and_run.sh
```
# To run the code 
```bash
cd build/
./Main -h # Show available options

# Example:
# Run OpenMP implementation of a newly generated matrix of
# dimension 32 with 2 threads
./Main -o -g -d 32 -p 2

# Run MPI implementation of a existing matrix of size 64
# on 4 MPI processes
mpirun -np 4 ./Main -m -d 64
```
# Testing correctness with SciPy
* Required packages: `NumPy`, `SciPy`
* Dimension (`dim`) needs to match the corresponding file
* Unfactored SPD matrix file and corresponding result output must be readily available.
* To run the test, do
  ```bash
  python test.py
  ``` 
  with the correct matrix dimension

# Cluster script generation
```bash
./scripts/submit_batch
python generate_submit_scripts.py
```
This will generate a series a subdirectories and submit scripts in sbatch format (weak scaling is currently disabled). Parameters can be customized by editing the `generate_submit_scripts.py` file.

# Cluster script submission
Before submission, the code must be compiled as the submission script does not automatically build the project. We recommend uncommenting all file output in `main.cpp` and `utils/cholesky.cpp` to avoid outputing redundant large matrices. 
```bash
./scripts/submit_batch # to show a list of available commands
./scripts/submit_batch -ma # mpi strong scaling
./scripts/submit_batch -oa # omp strong scaling
```
