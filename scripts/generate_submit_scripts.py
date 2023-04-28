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
                prepend="srun --mpi=pmi2 "
            )

def generate_omp_weak_scaling(num_threads, base_dim):
    dims = []
    for np in num_threads:
        dims.append(int(base_dim * (np / num_threads[0]) ** (1 / 3)))

    for np, dim in zip(num_threads, dims):
        write_submit_file(
            dir="single_node_weak/",
            key_string="omp_weak",
            run_flag="o",
            num_nodes=1,
            np=np,
            dim=dim,
        )
        
def generate_mpi_weak_scaling(num_threads, base_dim):
    dims = []
    for np in num_threads:
        dims.append(int(base_dim * (np / num_threads[0]) ** (1 / 3)))
    for np, dim in zip(num_threads, dims):
        write_submit_file(
            dir="across_node_weak/",
            key_string="mpi_weak",
            run_flag="m",
            num_nodes=np,
            np=np,
            dim=dim,
            prepend="srun --mpi=pmi2 "
        )


def write_submit_file(
    key_string: str,
    run_flag: str,
    num_nodes: int,
    np: int,
    dim: int,
    dir: str = "",
    prepend: str = "",
):
    ntasks = np * num_nodes
    
    filename = dir + f"submit_{key_string}_np_{ntasks}_dim_{dim}.sh"

    f = open(filename, "w")
    f.writelines(
        [
            "#!/bin/bash\n",
            f"#SBATCH --time=00:30:00\n",
            f"#SBATCH --nodes={num_nodes}\n",
            f"#SBATCH --ntasks-per-node={np}\n",
            f"#SBATCH --job-name=\"choleskey-benchmark-{key_string}-np-{ntasks}-dim-{dim}\"\n",
            f"#SBATCH --output=batchFiles/{key_string}-np-{ntasks}-dim-{dim}-%J.out\n",
            f"#SBATCH --error=batchFiles/{key_string}-np-{ntasks}-dim-{dim}-%J.err\n",
            f"#SBATCH -p secondary\n",
            "\n",
            f"cd $SLURM_SUBMIT_DIR/build\n",    # Assume called from source tree
            f"{prepend}./Main -{run_flag} -p {np} -d {dim} > $SLURM_SUBMIT_DIR/result/{key_string}-dim-{dim}-np-{ntasks}.txt\n"
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
        num_threads=[1, 2, 4, 8, 16],
        base_dim=512,
    )
    generate_mpi_weak_scaling(
        num_threads=[1, 2, 4, 8, 16],
        base_dim=512,
    )