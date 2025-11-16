import sys
import pandas as pd
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Usage: python3 plot_mc_results.py mc_results.csv")
    sys.exit(1)

df = pd.read_csv(sys.argv[1])
S_exact = 0.9445171858994637

# График 1: estimate vs N
plt.figure(figsize=(8,5))
for rect, grp in df.groupby('rect'):
    plt.plot(grp['N'], grp['estimate'], marker='.', linestyle='-', label=rect)
plt.hlines(S_exact, xmin=df['N'].min(), xmax=df['N'].max(), linestyles='--', label='S_exact')
plt.xlabel('N (число точек)')
plt.ylabel('Оценка площади S~')
plt.title('Оценка площади vs N')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('estimate_vs_N.png', dpi=300)

# График 2: relative error vs N (log-log)
plt.figure(figsize=(8,5))
for rect, grp in df.groupby('rect'):
    plt.loglog(grp['N'], grp['rel_error'], marker='.', linestyle='-', label=rect)
plt.xlabel('N (лог)')
plt.ylabel('Относительная ошибка (лог)')
plt.title('Относительная ошибка vs N (log-log)')
plt.legend()
plt.grid(True, which='both', ls='--')
plt.tight_layout()
plt.savefig('rel_error_vs_N.png', dpi=300)
