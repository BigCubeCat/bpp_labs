#!/bin/bash

python3 ./test/run.py ./test/config/count_process.json ~/22209/bit/lab5/res/
python3 ./test/run.py ./test/config/nodes.json ~/22209/bit/lab5/res/
python3 ./test/run.py ./test/config/profile.json ~/22209/bit/lab5/res/

watch squeue
