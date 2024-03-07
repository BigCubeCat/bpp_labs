#!/bin/python3
import os

size = 2000
start = 1
end = 4
versions = [
    'v0_sVector.c',
    'v0_reduction.c',
    'v1.c',
]

os.system("mkdir res")


def build(src: str, dest: str):
    return os.system(f"gcc {src} -O3 -fopenmp -o {dest}")


def read_time(file_name: str):
    with open(file_name, 'r', encoding='utf-8') as file:
        result = file.readline()
    return str(float(result)).replace(".", ",")


answer = [[str(i) for i in range(start - 1, end + 1)]]

for version in versions:
    # идем по строкам
    print("version = ", version)
    exe_file = "exe/" + version.replace('.c', ".exe")
    if build("src/" + version, exe_file) != 0:
        print("build error on ", "src/" + version)
        continue
    answer.append([version])
    for i in range(start, end + 1):
        # по столбцам
        file_name = f"res/{version}_{i}.txt"
        cmd = f"./{exe_file} {size} {i} > {file_name}"
        result = os.system(cmd)
        if result != 0:
            print("-------")
            print("ERROR!")
            print(result)
            print(cmd)
            print("-------")
        else:
            answer[-1].append(read_time(file_name))

table = [';'.join(answer[i]) for i in range(len(answer))]
with open("res/data.csv", 'w', encoding="utf-8") as file:
    file.write("\n".join(table))
