import numpy as np

ITERATIONS = 1e8


def price(iterations, S, K, r, v, T, is_call):
    x = np.random.normal(0, 1, int(iterations))
    data = S * np.exp((r - 0.5 * v * v) * T + v * np.sqrt(T) * x) - K
    if is_call:
        total = data[data > 0].sum()
    else:
        total = -data[data < 0].sum()
    return np.exp(-r * T) * total / iterations


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
