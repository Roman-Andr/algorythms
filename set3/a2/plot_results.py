import pandas as pd
import matplotlib.pyplot as plt

# Загружаем CSV
df = pd.read_csv('results.csv')

# Убираем строки с отсутствующим threshold, если нужно
df_hybrid = df[df['threshold'].notna()]
df_standard = df[df['threshold'].isna()]

# Строим графики времени работы стандартного Merge Sort
for typ, grp in df_standard.groupby('type'):
    plt.figure(figsize=(8,5))
    plt.plot(grp['n'], grp['mean_ms'], marker='o', label='mean_ms')
    plt.plot(grp['n'], grp['median_ms'], marker='x', label='median_ms')
    plt.fill_between(grp['n'], grp['min_ms'], grp['max_ms'], color='gray', alpha=0.2, label='min-max')
    plt.title(f'Standard Merge Sort - {typ}')
    plt.xlabel('Размер массива n')
    plt.ylabel('Время (ms)')
    plt.legend()
    plt.grid(True)
    plt.savefig(f'merge_{typ}.png')
    plt.close()

# Строим графики гибридного Merge+Insertion Sort
for typ, grp in df_hybrid.groupby('type'):
    plt.figure(figsize=(8,5))
    for th, g in grp.groupby('threshold'):
        plt.plot(g['n'], g['mean_ms'], marker='o', label=f'threshold={th}')
    plt.title(f'Hybrid Merge+Insertion Sort - {typ}')
    plt.xlabel('Размер массива n')
    plt.ylabel('Время (ms)')
    plt.legend()
    plt.grid(True)
    plt.savefig(f'hybrid_{typ}.png')
    plt.close()
