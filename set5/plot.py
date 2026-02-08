import pandas as pd
import matplotlib.pyplot as plt


def plot1():
    df = pd.read_csv("results.csv")
    plt.figure(figsize=(10, 6))
    plt.plot(df["percent"], df["mean_exact"], label="True Ft0", marker="o")
    plt.plot(df["percent"], df["mean_est"], label="Estimate Nt", marker="x")
    plt.xlabel("Percent of Stream")
    plt.ylabel("Number of Unique Elements")
    plt.title("Comparison of Estimate Nt and True Ft0")
    plt.legend()
    plt.grid(True)
    plt.savefig("graph1.png")


def plot2():
    df = pd.read_csv("results.csv")
    plt.figure(figsize=(10, 6))
    plt.plot(df["percent"], df["mean_est"], label="E(Nt)", color="blue")
    plt.fill_between(
        df["percent"],
        df["mean_est"] - df["std_est"],
        df["mean_est"] + df["std_est"],
        color="blue",
        alpha=0.2,
        label="E(Nt) ± σ",
    )
    plt.xlabel("Percent of Stream")
    plt.ylabel("Estimate")
    plt.title("Statistics of Estimate Nt")
    plt.legend()
    plt.grid(True)
    plt.savefig("graph2.png")


if __name__ == "__main__":
    plot1()
    plot2()
