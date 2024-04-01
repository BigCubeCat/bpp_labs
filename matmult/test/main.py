import json
import os
import sys

from TestSetup import TestSetup


def run_test(test: TestSetup):
    os.system(str(test))
    os.system(f"sleep {test.delay_after}")


def main(filename: str):
    with open(filename, "r", encoding="utf-8") as file:
        data = list(json.loads(file.read()))
    for test in data:
        run_test(TestSetup(test))


if __name__ == "__main__":
    try:
        main(sys.argv[1])
    except:
        main("config.json")
