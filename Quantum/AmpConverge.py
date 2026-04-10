import numpy as np

# Full simulator setup (pushQubit, tosQubit, applyGate, probQubit, measureQubit as defined in Part 1)
# Gate definitions
X_gate = np.array([[0, 1], [1, 0]])
H_gate = np.array([[1, 1], [1, -1]]) * np.sqrt(1/2)
Z_gate = H_gate @ X_gate @ H_gate

def sample_phaseOracle(qubits):   # marks target: all qubits 1 except qubit[1]
    applyGate(X_gate, qubits[1])
    applyGate(Z_gate, *namestack)
    applyGate(X_gate, qubits[1])

def zero_phaseOracle(qubits):
    [applyGate(X_gate, q) for q in qubits]
    applyGate(Z_gate, *namestack)
    [applyGate(X_gate, q) for q in qubits]

def groverSearch(n, printProb=True):
    qubits = list(range(n))
    [pushQubit(q, [1,1]) for q in qubits]
    for k in range(int(np.pi/4 * np.sqrt(2**n) - 1/2)):
        sample_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        zero_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        if printProb:
            p = probQubit(qubits[0])
            print(f"  Iter {k+1}: P(|0⟩)={p[0]:.6f}  P(|1⟩)={p[1]:.6f}")
    print("Result:", "".join([measureQubit(q) for q in reversed(qubits)]))

workspace = np.array([[1.]])
namestack = []
groverSearch(6)
