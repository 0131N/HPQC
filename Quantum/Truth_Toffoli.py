import numpy as np

workspace = np.array([[1.]])
namestack = []

def measureQubit(name):
    global workspace, namestack
    prob = probQubit(name)
    measurement = np.random.choice(2, p=prob)
    workspace = (workspace[:, [measurement]] / np.sqrt(prob[measurement]))
    namestack.pop()
    return str(measurement)

# Test: measure a biased qubit 1000 times and verify statistics
results = []
for _ in range(1000):
    workspace = np.array([[1.]]); namestack = []
    pushQubit("Q1", [0.6, 0.8])
    results.append(int(measureQubit("Q1")))

zeros = results.count(0)
ones  = results.count(1)
print(f"Out of 1000 measurements:")
print(f"  |0⟩: {zeros} times  (expected ~360, theoretical prob = 0.6² = 0.36)")
print(f"  |1⟩: {ones}  times  (expected ~640, theoretical prob = 0.8² = 0.64)")
