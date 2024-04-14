#!/bin/python3
import sys


def generate_file(filename, n, m, k):
    with open(filename, 'a', encoding="utf-8") as file:
        file.write(f"{n} {m} {k}\n")

    for i in range(n):
        with open(filename, 'a', encoding="utf-8") as file:
            file.write(" ".join([str(int(i == j)) for j in range(m)]) + "\n")

    for i in range(m):
        with open(filename, 'a', encoding="utf-8") as file:
            file.write(" ".join([str(int(i == j)) for j in range(k)]) + "\n")


def main():
    if len(sys.argv) != 5:
        print("Usage:")
        print("./input_generator.py <outfile.mat> <n> <m> <k>")
        return

    filename = sys.argv[1]
    n = int(sys.argv[2])
    m = int(sys.argv[3])
    k = int(sys.argv[4])
    generate_file(filename, n, m, k)


if __name__ == "__main__":
    main()
