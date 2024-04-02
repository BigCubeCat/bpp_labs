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
        self.input_path = script_config["input"]
        self.output_path = f'{script_config["output"]}_N{self.count_nodes}_n{self.count_mpi_process}'

        grid_config = data["grid"]
        self.custom = grid_config["custom"]
        if self.custom:
            self.grid_rows = grid_config["rows"]
            self.grid_columns = grid_config["columns"]
        else:
            self.grid_rows = 0
            self.grid_columns = 0

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
        result += f'{self.executable_path} {self.input_path} {self.output_path} deploy'
        if self.custom:
            result += f'{self.grid_rows} {self.grid_columns}'
        result += "\nEOF"

        return result

    def __str__(self):
        return self.generate_slurm_command()
