import numpy as np

workspace = np.array([[1.]])
namestack = []

def pushQubit(name, weights):
    global workspace, namestack
    if workspace.shape == (1, 1): namestack = []
    namestack.append(name)
    weights = np.array(weights / np.linalg.norm(weights), dtype=workspace[0, 0].dtype)
    workspace = np.kron(np.reshape(workspace, (1, -1)), weights)

# Test: push qubits and observe workspace doubling
for i in range(1, 7):
    workspace = np.array([[1.]])
    namestack = []
    for j in range(i):
        pushQubit(f"Q{j}", [1, 0])
    print(f"{i} qubit(s): workspace size = {workspace.size}")

