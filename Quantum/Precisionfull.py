import numpy as np
import time

# ── Simulator (dtype-aware) ────────────────────────────────────────────────
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
def groverSearch(n):
    global workspace, namestack
    qubits = list(range(n))
    [pushQubit(q, [1, 1]) for q in qubits]
    for _ in range(int(np.pi/4 * np.sqrt(2**n) - 0.5)):
        sample_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        zero_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
    return "".join([measureQubit(q) for q in reversed(qubits)])

# ── Run ────────────────────────────────────────────────────────────────────
n      = 8
target = "11111101"
trials = 50

print("=== Precision Comparison: float32 vs float64 ===")
print(f"N={n} qubits, target={target}, {trials} trials each\n")
print(f"{'Dtype':<12} {'Correct':<10} {'Success rate':<16} {'Total time':>12} {'Avg time/trial':>16}")
print("-" * 68)

for dtype in [np.float32, np.float64]:
    label = "float32" if dtype == np.float32 else "float64"
    successes = 0
    t0 = time.perf_counter()
    for _ in range(trials):
        workspace = np.array([[1.]], dtype=dtype)
        namestack = []
        result = groverSearch(n)
        if result == target:
            successes += 1
    elapsed = time.perf_counter() - t0
    avg_t   = elapsed / trials
    print(f"{label:<12} {successes}/{trials:<7} {successes/trials*100:>14.1f}%  "
          f"{elapsed:>10.4f} s  {avg_t*1000:>13.4f} ms")
