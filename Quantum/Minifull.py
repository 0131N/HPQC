import numpy as np
import time

# ── Minimalist Simulator (12 lines) ───────────────────────────────────────
def pushQubit(q, w):
    return np.kron(np.reshape(w, (1, -1)), q)

def applyGate(g, w):
    return np.matmul(np.reshape(w, (-1, g.shape[0])), g.T)

def tosQubit(k, w):
    return np.swapaxes(np.reshape(w, (-1, 2, 2**(k-1))), -2, -1)

def measureQubit(w):
    w = np.reshape(w, (-1, 2))
    p = np.linalg.norm(w, axis=0)
    m = np.random.choice(2, p=p**2)
    return (m, w[:, [m]] / p[m])

# ── Gates ──────────────────────────────────────────────────────────────────
n        = 10
X_gate   = np.array([[0, 1], [1, 0]])
H_gate   = np.array([[1, 1], [1, -1]]) * np.sqrt(1/2)
CZn_gate = np.diag(list(reversed(2 * np.sign(range(2**n)) - 1)))

# ── Oracles ────────────────────────────────────────────────────────────────
def sample_phaseOracle(w):
    w = applyGate(X_gate, tosQubit(2, w))
    w = applyGate(CZn_gate, tosQubit(2, w))
    w = applyGate(X_gate, tosQubit(2, w))
    return tosQubit(2, w)

def zero_phaseOracle(w):
    for i in range(n):
        w = applyGate(X_gate, tosQubit(n, w))
    w = applyGate(CZn_gate, w)
    for i in range(n):
        w = applyGate(X_gate, tosQubit(n, w))
    return w

# ── Grover Search ──────────────────────────────────────────────────────────
def groverSearch(w):
    for i in range(n):
        w = pushQubit(H_gate @ [1, 0], w)
    for k in range(int(np.pi/4 * np.sqrt(2**n) - 0.5)):
        w = sample_phaseOracle(w)
        for i in range(n):
            w = applyGate(H_gate, tosQubit(n, w))
        w = zero_phaseOracle(w)
        for i in range(n):
            w = applyGate(H_gate, tosQubit(n, w))
    result = []
    for i in range(n):
        (m, w) = measureQubit(tosQubit(n - i, w))
        result.append(str(m))
    return "".join(result)

# ── Run ────────────────────────────────────────────────────────────────────
print("=== Minimalist Simulator (12-line core) ===")
print(f"N={n} qubits\n")

trials = 20
print(f"Running {trials} trials...\n")

t0 = time.perf_counter()
for i in range(trials):
    result = groverSearch(np.array([[1.]]))
    print(f"  Trial {i+1:>2}: {result}")

elapsed = time.perf_counter() - t0
print(f"\nTotal time : {elapsed:.4f} s")
print(f"Avg / trial: {elapsed/trials*1000:.4f} ms")
