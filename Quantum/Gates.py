import numpy as np

workspace = np.array([[1.]])
namestack = []

X_gate = np.array([[0, 1], [1, 0]])
H_gate = np.array([[1, 1], [1, -1]]) * np.sqrt(1/2)
SWAP_gate = np.array([[1,0,0,0],[0,0,1,0],[0,1,0,0],[0,0,0,1]])

def applyGate(gate, *names):
    global workspace
    if list(names) != namestack[-len(names):]:
        [tosQubit(n) for n in names]
    workspace = np.reshape(workspace, (-1, gate.shape[0]))
    subworkspace = workspace[:, -gate.shape[0]:]
    np.matmul(subworkspace, gate.T, out=subworkspace)

# Test 1: X gate (NOT)
workspace = np.array([[1.]]); namestack = []
pushQubit("Q1", [1, 0])
print("Before X:", workspace)
applyGate(X_gate, "Q1")
print("After X: ", workspace)

# Test 2: H gate (superposition)
workspace = np.array([[1.]]); namestack = []
pushQubit("Q1", [1, 0])
print("\nBefore H:", workspace)
applyGate(H_gate, "Q1")
print("After H: ", workspace)
print("Prob |0⟩:", round(workspace[0,0]**2, 4), "Prob |1⟩:", round(workspace[0,1]**2, 4))

# Test 3: SWAP gate (two qubits)
workspace = np.array([[1.]]); namestack = []
pushQubit("Q1", [1, 0])
pushQubit("Q2", [0.6, 0.8])
print("\nBefore SWAP:", np.reshape(workspace, (1,-1)))
applyGate(SWAP_gate, "Q1", "Q2")
print("After SWAP: ", np.reshape(workspace, (1,-1)))
