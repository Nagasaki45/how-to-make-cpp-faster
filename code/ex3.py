import numpy as np
import pandas as pd


N = 2 ** 20

# Task 1
df = pd.DataFrame({
    'inputdata': np.random.normal(500, 200, size=N).astype(int),
    'inputkeys': np.random.randint(20, size=N),
})

print(f'Average:       {df.inputdata.mean()}')
print(f'Std:           {df.inputdata.std()}')
print(f'Contains 1234: {1234 in df.inputdata.values}')

# Task 2
print(df.groupby(by='inputkeys').aggregate([len, sum]))

# Task 3
print(
    df
    .sort_values(by='inputdata', ascending=False)
    .head(20)
    .drop_duplicates('inputkeys')
)
