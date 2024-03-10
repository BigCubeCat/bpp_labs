#!/bin/python3

size = 100


def generate_line(index, size):
    return " ".join([str(1 + int(index == i)) for i in range(size)]) + '\n'


def generate_b(size):
    return " ".join([str(size + 1) for i in range(size)]) + '\n'


def write(filename, string):
    with open(filename, "a") as file:
        file.write(string)


if __name__ == "__main__":
    name = "test.data"
    write(name, str(size) + '\n')
    for i in range(size):
        write(name, generate_line(i, size))
    write(name, generate_b(size))

