#!/bin/python3
import os

size = 25000
count_cols = 5
end = 16
versions = [
    'v1.c',
    #'v0_reduction.c',
]

configurations = [
    'static, 1',
    'static, 2',
    'static, 4',
    'static, 8',
    'static, 16',
    'dynamic, 1',
    'dynamic, 2',
    'dynamic, 4',
    'dynamic, 8',
    'dynamic, 16',
    'guided, 1',
    'guided, 2',
    'guided, 4',
    'guided, 8',
    'guided, 16'
] 

os.system("mkdir res")
os.system("mkdir exe")


def change_file(filename: str, config: str) -> str:
    result_file = (filename + config.replace(", ", "") + '.c').replace("src", "exe", 1)
    with open(filename, 'r') as read_file:
        data = read_file.read()
    src = data.replace("@", config)
    with open(result_file, 'w') as file:
        file.write(src)
    return result_file


def build(src: str, dest: str):
    return os.system(f"gcc {src} -O2 -fopenmp -o {dest}")


def read_time(file_name: str):
    with open(file_name, 'r', encoding='utf-8') as file:
        result = file.readline()
    return str(float(result)).replace(".", ",")

def run_test(config: str):
    print("config = ", config)
    pretty_config = config.replace(", ", "");
    answer = [[str(i) for i in range(count_cols)]]
    for version in versions:
        # идем по строкам
        print("version = ", version)
        version = "src/" + version
        src_file = change_file(version, config)
        exe_file = src_file.replace('.c', ".exe")
        if build(src_file, exe_file) != 0:
            print("build error on ", src_file)
            continue
        answer.append([config])
        i = 1;
        while (i < end):
            print("count threads: ", i)
            # по столбцам
            file_name = f"res/{pretty_config}_{i}.txt"
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
            i *= 2
    table = [';'.join(answer[i]) for i in range(len(answer))]
    with open("res/data(" + pretty_config + ").csv", 'w', encoding="utf-8") as file:
        file.write("\n".join(table))


if __name__ == "__main__":
    for config in configurations:
        run_test(config)

