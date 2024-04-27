import os

from . import config_reader
from .ClusterTesting import ClusterTesting


def abstract_run(config_path, project_name, out_path, processor):
    test_conf = config_reader.read_config(config_path, project_name, out_path)
    clusterTest = ClusterTesting(test_conf, processor)
    clusterTest.run_all()


def bash_run(config_path, project_name, out_path):
    abstract_run(config_path, project_name, out_path, os.system)


def debug_run(config_path, project_name, out_path):
    abstract_run(config_path, project_name, out_path, print)
