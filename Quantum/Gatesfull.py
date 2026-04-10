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
X_gate    = np.array([[0, 1], [1, 0]])
H_gate    = np.array([[1, 1], [1, -1]]) * np.sqrt(1/2)
Z_gate    = np.array([[1, 0], [0, -1]])
SWAP_gate = np.array([[1,0,0,0],[0,0,1,0],[0,1,0,0],[0,0,0,1]])
CNOT_gate = np.array([[1,0,0,0],[0,1,0,0],[0,0,0,1],[0,0,1,0]])
TOFF_gate = np.array([
    [1,0,0,0,0,0,0,0],[0,1,0,0,0,0,0,0],[0,0,1,0,0,0,0,0],[0,0,0,1,0,0,0,0],
    [0,0,0,0,1,0,0,0],[0,0,0,0,0,1,0,0],[0,0,0,0,0,0,0,1],[0,0,0,0,0,0,1,0]
])

def reset():
    global workspace, namestack
    workspace = np.array([[1.]])
    namestack = []

# ── Test 1: X Gate (NOT) ───────────────────────────────────────────────────
print("=== Test 1: X Gate (NOT) ===")
reset()
pushQubit("Q1", [1, 0])
print(f"  Before X: {np.reshape(workspace,(1,-1))}")
applyGate(X_gate, "Q1")
print(f"  After X:  {np.reshape(workspace,(1,-1))}")
print()

# ── Test 2: H Gate (Superposition) ────────────────────────────────────────
print("=== Test 2: H Gate (Hadamard) ===")
reset()
pushQubit("Q1", [1, 0])
print(f"  Before H: {np.reshape(workspace,(1,-1))}")
applyGate(H_gate, "Q1")
ws = np.reshape(workspace,(1,-1))
print(f"  After H:  {ws}")
print(f"  P(|0>)={ws[0,0]**2:.4f}  P(|1>)={ws[0,1]**2:.4f}")
print()

# ── Test 3: SWAP Gate ──────────────────────────────────────────────────────
print("=== Test 3: SWAP Gate ===")
reset()
pushQubit("Q1", [1, 0])
pushQubit("Q2", [0.6, 0.8])
print(f"  Before SWAP: {np.reshape(workspace,(1,-1))}")
applyGate(SWAP_gate, "Q1", "Q2")
print(f"  After SWAP:  {np.reshape(workspace,(1,-1))}")
print()

# ── Test 4: CNOT Gate ──────────────────────────────────────────────────────
print("=== Test 4: CNOT Gate (XOR) ===")
print(f"  {'Q1':<6} {'Q2':<6} {'Q2 after CNOT':>14}")
print("  " + "-" * 28)
for q1_state, q2_state in [([1,0],[1,0]), ([1,0],[0,1]), ([0,1],[1,0]), ([0,1],[0,1])]:
    reset()
    pushQubit("Q1", q1_state)
    pushQubit("Q2", q2_state)
    applyGate(CNOT_gate, "Q1", "Q2")
    q2_out = measureQubit("Q2")
    q1_out = measureQubit("Q1")
    print(f"  {q1_out:<6} {str(q2_state):<6} {q2_out:>14}")
print()

# ── Test 5: Toffoli AND Truth Table ───────────────────────────────────────
print("=== Test 5: Toffoli Gate (AND) Truth Table ===")
print(f"  {'Q1':<6} {'Q2':<6} {'Q3=AND':>8}")
print("  " + "-" * 22)
for _ in range(16):
    reset()
    pushQubit("Q1", [1, 0])
    applyGate(H_gate, "Q1")
    pushQubit("Q2", [1, 0])
    applyGate(H_gate, "Q2")
    pushQubit("Q3", [1, 0])
    applyGate(TOFF_gate, "Q1", "Q2", "Q3")
    q3 = measureQubit("Q3")
    q2 = measureQubit("Q2")
    q1 = measureQubit("Q1")
    print(f"  {q1:<6} {q2:<6} {q3:>8}")
print()

# ── Test 6: Measurement Statistics ────────────────────────────────────────
print("=== Test 6: Measurement Statistics (1000 trials) ===")
print("  Qubit initialised with weights [0.6, 0.8]")
print("  Expected: P(|0>)=0.36, P(|1>)=0.64\n")
results = []
t0 = time.perf_counter()
for _ in range(1000):
    reset()
    pushQubit("Q1", [0.6, 0.8])
    results.append(int(measureQubit("Q1")))
elapsed = time.perf_counter() - t0
zeros = results.count(0)
ones  = results.count(1)
print(f"  |0> measured: {zeros:>4} times  (expected ~360)")
print(f"  |1> measured: {ones:>4} times  (expected ~640)")
print(f"  Time for 1000 measurements: {elapsed*1000:.2f} ms")
