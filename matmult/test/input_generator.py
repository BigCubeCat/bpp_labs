import random


def random_value():
    return random.randint(100, 1000)


OUT_FILENAME = "../input.mat"
n = 25000
m = 25000
k = 25000

with open(OUT_FILENAME, 'a', encoding="utf-8") as file:
    file.write(f"{n} {m} {k}\n")

for i in range(n):
    with open(OUT_FILENAME, 'a', encoding="utf-8") as file:
        file.write(" ".join([str(bool(i == j)) for j in range(m)]) + "\n")

for i in range(m):
    with open(OUT_FILENAME, 'a', encoding="utf-8") as file:
        file.write(" ".join([str(bool(i == j)) for j in range(k)]) + "\n")
