import math
import random

from numba import jit

ITERATIONS = 1e8


@jit
def price(iterations, S, K, r, v, T, is_call):
    accumulator = 0;
    for _ in range(iterations):
        x = random.gauss(0, 1)
        datum = S * math.exp((r - 0.5 * v * v) * T + v * math.sqrt(T) * x)
        if is_call:
            accumulator += max(datum - K, 0.0);
        else:
            accumulator += max(K - datum, 0.0);
    return math.exp(-r * T) * accumulator / iterations;


# Some constants
S = 100.0;  # Option price
K = 100.0;  # Strike price
r = 0.05;   # Risk-free rate (5%)
v = 0.2;    # Volatility of the underlying (20%)
T = 1.0;    # One year until expiry

call = price(ITERATIONS, S, K, r, v, T, True);
put = price(ITERATIONS, S, K, r, v, T, False);

print(f'Underlying:      {S}')
print(f'Strike:          {K}')
print(f'Risk-Free Rate:  {r}')
print(f'Volatility:      {v}')
print(f'Maturity:        {T}')
print(f'Call Price:      {call}')
print(f'Put Price:       {put}')
