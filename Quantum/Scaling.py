import numpy as np, math

print(f"{'N qubits':<12} {'Search space':<16} {'Optimal iterations':<22} {'Classical avg':<20}")
print("-" * 70)
for n in range(2, 17):
    space = 2**n
    k_opt = int(np.pi/4 * math.sqrt(space) - 0.5)
    classical = space // 2
    print(f"{n:<12} {space:<16} {k_opt:<22} {classical:<20}")
