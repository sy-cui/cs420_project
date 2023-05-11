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
* Unfactored SPD matrix file and corresponding result output must be readily available. To generated result outputs, uncomment the file-io of respective functions in `main.cpp`.
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
```bash
./scripts/submit_batch # to show a list of available commands
./scripts/submit_batch -ma # mpi strong scaling
./scripts/submit_batch -oa # omp strong scaling
```
