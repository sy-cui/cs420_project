import numpy as np
import scipy as sp

dim = 32
a = np.loadtxt(f"data/size{dim}.txt")
b = np.zeros((dim, dim))
for i in range(dim):
    for j in range(dim):
        if i >= j:
            b[i, j] = a[i * (i + 1) // 2 + j]
            b[j, i] = b[i, j]

c = np.loadtxt("result.txt")
result = np.zeros((dim, dim))
for i in range(dim):
    for j in range(dim):
        if i >= j:
            result[i, j] = c[i * (i + 1) // 2 + j]

soln = sp.linalg.cholesky(b).T
print(result)
print(soln)
print(np.linalg.norm(soln - result))