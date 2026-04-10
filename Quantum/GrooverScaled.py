import numpy as np, torch as pt, time
pt.autograd.set_grad_enabled(False)


results = []
for n in [8, 10, 12, 14, 16]:
    for device_str in ['cpu', 'cuda']:
        if device_str == 'cuda' and not pt.cuda.is_available():
            continue
        workspace = pt.tensor([[1.]], device=pt.device(device_str), dtype=pt.float32)
        namestack = []
        t0 = time.process_time()
        groverSearch(n, printProb=False)
        elapsed = time.process_time() - t0
        results.append((n, device_str, elapsed))
        print(f"N={n}, {device_str.upper():>4}: {elapsed:.4f} s")
