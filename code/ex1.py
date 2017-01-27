import random

from numba import jit

ITERATIONS = 1e8


@jit
def simulate(iterations):
    inside = 0
    for _ in range(iterations):
        x = random.random() * 2 - 1
        y = random.random() * 2 - 1
        if x * x + y * y < 1:
            inside += 1
    return inside / iterations * 4


pi = simulate(ITERATIONS)
print(f'pi estimation after {ITERATIONS} iterations: {pi}')
