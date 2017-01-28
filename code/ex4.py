import pandas as pd

df = pd.DataFrame({
    'day':      [0,0,0,1,2,2,5,5,5,5,6,7,7,9,9,9,10,10,11,12,14,14],
    'position': [2,3,0,1,1,2,3,2,0,1,2,3,0,2,1,3,0, 2, 2, 1, 0, 1 ],
    'amount':   [1,3,8,3,4,3,1,2,2,5,7,2,5,9,1,4,7, 6, 10,2, 1, 8 ],
})

# Step 2: number of days with any measurements
print('Unique days:', len(df.day.unique()))

# Step 3: total amount at each site
for position, total in df.groupby(by='position').amount.sum().iteritems():
    print(f'Position {position} total: {total}')

# Step 4: number of days where total amount exceeded 6
day_amount = df.groupby(by='day').amount.sum()
days_exceeded6 = len(day_amount[day_amount > 6])
print('# of days where total amount exceeded 6:', days_exceeded6)

# Step 5: first day where total amount (so far) exceeded 30
cumsum_amount = day_amount.cumsum()
exceeded30 = cumsum_amount[cumsum_amount > 30].index[0]
print('First day where cumsum exceeded 30:', exceeded30)

# Step 6: total amount between days 4 and 10
total = day_amount[(day_amount.index >= 4) & (day_amount.index <= 10)].sum()
print('Total between days 4 and 10:', total)
