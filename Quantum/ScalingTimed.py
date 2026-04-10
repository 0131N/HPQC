import numpy as np
import time

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

def groverSearch(n):
    global workspace, namestack
    workspace = np.array([[1.]])
    namestack = []
    qubits = list(range(n))
    [pushQubit(q, [1, 1]) for q in qubits]
    for _ in range(int(np.pi/4 * np.sqrt(2**n) - 0.5)):
        sample_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        zero_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
    return "".join([measureQubit(q) for q in reversed(qubits)])

# ── Run ────────────────────────────────────────────────────────────────────
print("=== Grover Scaling: Optimal Iterations vs Search Space ===\n")
print(f"{'N qubits':<12} {'Search space':<16} {'Optimal iters':<16} "
      f"{'Classical avg':<16} {'Sim time (s)':>12} {'Speedup ratio':>14}")
print("-" * 90)

for n in range(2, 17):
    space     = 2**n
    k_opt     = int(np.pi/4 * np.sqrt(space) - 0.5)
    classical = space // 2
    speedup   = classical / k_opt if k_opt > 0 else float('inf')

    t0      = time.perf_counter()
    result  = groverSearch(n)
    elapsed = time.perf_counter() - t0

    print(f"{n:<12} {space:<16} {k_opt:<16} {classical:<16} {elapsed:>12.4f} {speedup:>14.1f}x")
