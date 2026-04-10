import numpy as np

# Run Grover's on 6 qubits at different iteration counts and record success rate
n = 6
target = "111101"

def groverFixed(n, iterations):
    qubits = list(range(n))
    [pushQubit(q, [1,1]) for q in qubits]
    for _ in range(iterations):
        sample_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        zero_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
    return "".join([measureQubit(q) for q in reversed(qubits)])

trials = 100
print(f"{'Iterations':<14} {'Success rate':>14}")
print("-" * 30)
for iters in [1, 2, 3, 4, 5, 6, 7]:
    successes = 0
    for _ in range(trials):
        workspace = np.array([[1.]]); namestack = []
        if groverFixed(n, iters) == target:
            successes += 1
    print(f"{iters:<14} {successes/trials*100:>13.1f}%")
