#!/bin/python3
import os
import sys

if len(sys.argv) < 4:
    print("usage: collect_data.py <folder> <extension>")
    sys.exit()

folder_path = sys.argv[1]
file_extension = sys.argv[2]
res_path = sys.argv[3]

files = os.listdir(folder_path)

header = []
data = []

for file in files:
    if file.endswith(file_extension):
        with open(os.path.join(folder_path, file), 'r') as f:
            content = f.readline()
            try:
                res = float(content)
                header.append(file)
                data.append(str(res).replace(".", ","))
            except:
                pass

file_content = ';'.join(header) + '\n' + ';'.join(data) + '\n'
with open(res_path, "w", encoding="utf-8") as res_file: 
    res_file.write(file_content)


