import numpy as np

# Appendix B: Complete minimalist implementation
def pushQubit(q, w):  return np.kron(np.reshape(w, (1,-1)), q)
def applyGate(g, w):  return np.matmul(np.reshape(w, (-1, g.shape[0])), g.T)
def tosQubit(k, w):   return np.swapaxes(np.reshape(w, (-1,2,2**(k-1))), -2, -1)
def measureQubit(w):
    w = np.reshape(w, (-1, 2))
    p = np.linalg.norm(w, axis=0)
    m = np.random.choice(2, p=p**2)
    return (m, w[:,[m]]/p[m])

# Run and verify identical output to full implementation
n = 10
X_gate = np.array([[0,1],[1,0]])
H_gate = np.array([[1,1],[1,-1]]) * np.sqrt(1/2)
CZn_gate = np.diag(list(reversed(2*np.sign(range(2**n))-1)))

# minimalist groverSearch as defined in Appendix B
groverSearch(np.array([[1.]]))
