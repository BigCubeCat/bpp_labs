class TestSetup:
    def __init__(self, data: dict):
        slurm_config = data["slurm_config"]
        self.job_name = slurm_config["job"]
        self.time = slurm_config["time"]
        self.p = "compclass"
        self.out = slurm_config["out"]
        self.err = slurm_config["err"]
        self.count_nodes = slurm_config["nodes"]
        self.count_mpi_process = slurm_config["mpi_process"]
        self.time = slurm_config["time"]
        self.is_trace = slurm_config["trace"]
        self.delay_after = slurm_config["delay"]

        script_config = data["script_config"]
        self.executable_path = script_config["exe"]

    def generate_slurm_command(self) -> str:
        result = f'sbatch -J {self.job_name} -p {self.p} -o {self.out} -e {self.err} '
        result += f'-N {self.count_nodes} -n {self.count_mpi_process} '
        result += f'-t {self.time} <<EOF\n'
        result += "#!/bin/bash\n"
        result += 'module load mpi/intelmpi\n'
        result += 'module load itac\n'
        result += 'export I_MPI_PORT_RANGE=23000:23100\n'
        result += 'mpirun '
        if self.is_trace:
            result += '-trace '
        result += f'-ppn {self.count_nodes} -n {self.count_mpi_process} '
        result += self.executable_path
        result += "\nEOF"

        return result

    def __str__(self):
        return self.generate_slurm_command()
