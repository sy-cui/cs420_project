def generate_omp_strong_scaling(num_threads, dims):
    for np in num_threads:
        for dim in dims:
            write_submit_file(
                dir="single_node_strong/",
                key_string="omp_strong",
                run_flag="o",
                num_nodes=1,
                np=np,
                dim=dim,
            )

def generate_mpi_strong_scaling(num_threads, dims):
    for np in num_threads:
        for dim in dims:
            write_submit_file(
                dir="across_node_strong/",
                key_string="mpi_strong",
                run_flag="m",
                num_nodes=np,
                np=np,
                dim=dim,
            )

def generate_omp_weak_scaling(num_threads, dims):
    assert len(num_threads) == len(dims), (
        "For weak scaling, num_threads and dims must have the same length"
    )
    for np, dim in zip(num_threads, dims):
        write_submit_file(
            dir="single_node_weak/",
            key_string="omp_weak",
            run_flag="o",
            num_nodes=1,
            np=np,
            dim=dim,
        )
        
def generate_mpi_weak_scaling(num_threads, dims):
    assert len(num_threads) == len(dims), (
        "For weak scaling, num_threads and dims must have the same length"
    )
    for np, dim in zip(num_threads, dims):
        write_submit_file(
            dir="across_node_weak/",
            key_string="mpi_weak",
            run_flag="m",
            num_nodes=np,
            np=np,
            dim=dim,
        )


def write_submit_file(
    key_string: str,
    run_flag: str,
    num_nodes: int,
    np: int,
    dim: int,
    dir: str = "",
):
    filename = dir + f"submit_{key_string}_np_{np}_dim_{dim}.sh"

    f = open(filename, "w")
    f.writelines(
        [
            "#!/bin/bash\n",
            f"#SBATCH --time=00:30:00\n",
            f"#SBATCH --nodes={num_nodes}\n",
            f"#SBATCH --ntasks={np}\n",
            f"#SBATCH --job-name=\"choleskey-benchmark-{key_string}-np-{np}-dim-{dim}\"\n",
            f"#SBATCH --output=batchFiles/{key_string}-np-{np}-dim-{dim}-%J.out\n",
            f"#SBATCH --error=batchFiles/{key_string}-np-{np}-dim-{dim}-%J.err\n",
            f"#SBATCH -p secondary\n",
            "\n",
            f"cd $SLURM_SUBMIT_DIR/build\n",    # Assume called from source tree
            f"./Main -{run_flag} -p {np} -d {dim} > $SLURM_SUBMIT_DIR/result/{key_string}-dim-{dim}-np-{np}.txt\n"
        ]
    )
    f.close()

if __name__ == "__main__":
    generate_omp_strong_scaling(
        num_threads=[1, 2, 4, 8, 16],
        dims=[4096],
    )
    generate_mpi_strong_scaling(
        num_threads=[1, 2, 4, 8, 16],
        dims=[4096],
    )
    generate_omp_weak_scaling(
        num_threads=[1, 4, 16],
        dims=[1024, 2048, 4096],
    )
    generate_mpi_weak_scaling(
        num_threads=[1, 4, 16],
        dims=[1024, 2048, 4096],
    )