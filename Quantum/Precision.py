import numpy as np, torch as pt
pt.autograd.set_grad_enabled(False)

n = 8
target = "11111101"
trials = 50

for dtype in [pt.float32, pt.float64]:
    successes = 0
    for _ in range(trials):
        workspace = pt.tensor([[1.]], dtype=dtype)
        namestack = []
        groverSearch(n, printProb=False)  # uses PyTorch version
        # check result string against target (adapt measureQubit to return str)
        # successes += (result == target)
    label = "float32" if dtype == pt.float32 else "float64"
    print(f"{label}: {successes}/{trials} correct  ({successes/trials*100:.1f}%)")
