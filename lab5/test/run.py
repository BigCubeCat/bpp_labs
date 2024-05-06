import sys

import ClusterTesting

if __name__ == "__main__":
    ClusterTesting.bash_run(
        sys.argv[1],
        "cluster",
        sys.argv[2]
    )
