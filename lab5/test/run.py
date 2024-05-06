import sys

import ClusterTesting

if __name__ == "__main__":
    ClusterTesting.debug_run(
        sys.argv[1],
        "cluster",
        sys.argv[2]
    )
