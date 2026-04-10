import numpy as np

# ── Simulator ──────────────────────────────────────────────────────────────
workspace = np.array([[1.]])
namestack = []

def pushQubit(name, weights):
    global workspace, namestack
    if workspace.shape == (1, 1):
        namestack = []
    namestack.append(name)
    weights = np.array(weights / np.linalg.norm(weights), dtype=workspace[0, 0].dtype)
    workspace = np.kron(np.reshape(workspace, (1, -1)), weights)

def tosQubit(name):
    global workspace, namestack
    k = len(namestack) - namestack.index(name)
    if k > 1:
        namestack.append(namestack.pop(-k))
        workspace = np.reshape(workspace, (-1, 2, 2**(k-1)))
        workspace = np.swapaxes(workspace, -2, -1)

def applyGate(gate, *names):
    global workspace
    if list(names) != namestack[-len(names):]:
        [tosQubit(n) for n in names]
    workspace = np.reshape(workspace, (-1, 2**len(names)))
    subworkspace = workspace[:, -gate.shape[0]:]
    np.matmul(subworkspace, gate.T, out=subworkspace)

def probQubit(name):
    global workspace
    tosQubit(name)
    workspace = np.reshape(workspace, (-1, 2))
    prob = np.linalg.norm(workspace, axis=0)**2
    return prob / prob.sum()

def measureQubit(name):
    global workspace, namestack
    prob = probQubit(name)
    measurement = np.random.choice(2, p=prob)
    workspace = workspace[:, [measurement]] / np.sqrt(prob[measurement])
    namestack.pop()
    return str(measurement)

# ── Gates ──────────────────────────────────────────────────────────────────
X_gate = np.array([[0, 1], [1, 0]])
H_gate = np.array([[1, 1], [1, -1]]) * np.sqrt(1/2)
Z_gate = H_gate @ X_gate @ H_gate

# ── Oracles ────────────────────────────────────────────────────────────────
def sample_phaseOracle(qubits):
    applyGate(X_gate, qubits[1])
    applyGate(Z_gate, *namestack)
    applyGate(X_gate, qubits[1])

def zero_phaseOracle(qubits):
    [applyGate(X_gate, q) for q in qubits]
    applyGate(Z_gate, *namestack)
    [applyGate(X_gate, q) for q in qubits]

# ── Grover Search ──────────────────────────────────────────────────────────
def groverSearch(n, printProb=True):
    global workspace, namestack
    workspace = np.array([[1.]])
    namestack = []
    qubits = list(range(n))
    [pushQubit(q, [1, 1]) for q in qubits]
    for k in range(int(np.pi/4 * np.sqrt(2**n) - 0.5)):
        sample_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        zero_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        if printProb:
            p = probQubit(qubits[0])
            print(f"  Iter {k+1}: P(|0>)={p[0]:.6f}  P(|1>)={p[1]:.6f}")
    result = "".join([measureQubit(q) for q in reversed(qubits)])
    print(f"Result: {result}")

# ── Run ────────────────────────────────────────────────────────────────────
import time
print("=== Amplitude Convergence Test (N=6) ===")
t0 = time.perf_counter()
groverSearch(6)
print(f"Time: {time.perf_counter() - t0:.4f} s\n")
