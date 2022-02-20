#!/usr/bin/python3

from operator import itemgetter
import sys

ar, br = [], []
mats = {
    'A': {},
    'B': {},
}

# read the entire line from STDIN
for line in sys.stdin:
    # remove leading and trailing whitespace
    line = line.strip()
    # splitting the data on the basis of tab we have provided in mapper.py
    inp_line = line.split(' ')
    if inp_line[0] == 'AR':
        ar = inp_line[1:]
        continue
    elif inp_line[0] == 'BR':
        br = inp_line[1:]
        continue
    mats[inp_line[0]][(int(inp_line[1]), int(inp_line[2]))] = inp_line[3]

# initialize the matrix of size (ar[0], br[1])
mat = [[0 for _ in range(int(br[1]))] for _ in range(int(ar[0]))]
for i in range(int(ar[0])):
    for j in range(int(br[0])):
        for k in range(int(br[1])):
            mat[i][k] += int(mats['A'][(i, j)]) * int(mats['B'][(j, k)])

# print the matrix
for i in range(int(ar[0])):
    for j in range(int(br[1])):
        print(mat[i][j], end=' ')
    print()
