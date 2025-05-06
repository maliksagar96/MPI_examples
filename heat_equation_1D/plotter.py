import matplotlib.pyplot as plt
import numpy as np
import os

# Ensure output directory exists
os.makedirs("images", exist_ok=True)

nx = 1000
x = np.linspace(0, 2, nx)

for t in range(1000):
    filename = f"data/u_{t}.txt"
    try:
        u = np.loadtxt(filename)
        plt.figure(figsize=(8, 4))
        plt.plot(x, u)
        plt.title(f"Time Step {t}")
        plt.xlabel("x")
        plt.ylabel("u")
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(f"images/u_{t}.png")
        plt.close()
    except Exception as e:
        print(f"Error reading {filename}: {e}")
