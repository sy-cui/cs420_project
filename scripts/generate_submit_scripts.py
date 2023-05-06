def generate_omp_strong_scaling(num_threads, dims):
    for np in num_threads:
        for dim in dims:
            write_submit_file(
                dir="single_node_strong/",
                key_string="omp_strong",
                run_flag="o",
                num_nodes=1,
                num_tasks_per_node=1,
                num_cpus_per_task=np,
                dim=dim,
            )

def generate_mpi_strong_scaling(num_threads, dims):
    max_tasks_per_node = 17
    num_nodes = 1
    num_tasks_per_node = 1

    for np in num_threads:
        for dim in dims:
            num_nodes = min(2 ** (np // max_tasks_per_node), 4)
            num_tasks_per_node = np // num_nodes
            write_submit_file(
                dir="across_node_strong/",
                key_string="mpi_strong",
                run_flag="m",
                num_nodes=num_nodes,
                num_tasks_per_node=num_tasks_per_node,
                num_cpus_per_task=1,
                dim=dim,
                prepend="srun --mpi=pmi2 "
            )

# def generate_omp_weak_scaling(num_threads, base_dim):
#     dims = []
#     for np in num_threads:
#         dims.append(int(base_dim * (np / num_threads[0]) ** (1 / 3)))

#     for np, dim in zip(num_threads, dims):
#         write_submit_file(
#             dir="single_node_weak/",
#             key_string="omp_weak",
#             run_flag="o",
#             num_nodes=1,
#             np=np,
#             dim=dim,
#         )
        
# def generate_mpi_weak_scaling(num_threads, base_dim):
#     dims = []
#     for np in num_threads:
#         dims.append(int(base_dim * (np / num_threads[0]) ** (1 / 3)))
#     for np, dim in zip(num_threads, dims):
#         write_submit_file(
#             dir="across_node_weak/",
#             key_string="mpi_weak",
#             run_flag="m",
#             num_nodes=np,
#             np=1,
#             dim=dim,
#             prepend="srun --mpi=pmi2 "
#         )


def write_submit_file(
    key_string: str,
    run_flag: str,
    num_nodes: int,
    num_tasks_per_node: int,
    num_cpus_per_task: int,
    dim: int,
    dir: str = "",
    prepend: str = "",
):
    num_threads = num_nodes * num_tasks_per_node * num_cpus_per_task
    
    filename = dir + f"submit_{key_string}_np_{num_threads}_dim_{dim}.sh"

    f = open(filename, "w")
    f.writelines(
        [
            "#!/bin/bash\n",
            f"#SBATCH --time=00:30:00\n",
            f"#SBATCH --nodes={num_nodes}\n",
            f"#SBATCH --ntasks-per-node={num_tasks_per_node}\n",
            f"#SBATCH --cpus-per-task={num_cpus_per_task}\n",
            f"#SBATCH --job-name=\"choleskey-benchmark-{key_string}-np-{num_threads}-dim-{dim}\"\n",
            f"#SBATCH --output=batchFiles/{key_string}-np-{num_threads}-dim-{dim}-%J.out\n",
            f"#SBATCH --error=batchFiles/{key_string}-np-{num_threads}-dim-{dim}-%J.err\n",
            f"#SBATCH -p secondary\n",
            "\n",
            f"cd $SLURM_SUBMIT_DIR/build\n",    # Assume called from source tree
            f"{prepend}./Main -{run_flag} -p {num_cpus_per_task} -d {dim} "
            f"> $SLURM_SUBMIT_DIR/result/{key_string}-dim-{dim}-np-{num_threads}.txt\n"
        ]
    )
    f.close()

if __name__ == "__main__":
    generate_omp_strong_scaling(
        num_threads=[1, 2, 4, 8, 16],
        dims=[4096],
    )
    generate_mpi_strong_scaling(
        num_threads=[1, 2, 4, 8, 16, 32, 64],
        dims=[4096],
    )
    # generate_omp_weak_scaling(
    #     num_threads=[1, 2, 4, 8, 16],
    #     base_dim=512,
    # )
    # generate_mpi_weak_scaling(
    #     num_threads=[1, 2, 4, 8, 16],
    #     base_dim=512,
    # )