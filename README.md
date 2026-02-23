
# Topic 2: Performance and Parallelism – Lab Report

## 1. Introduction

This project explores the performance differences between Compiled and Interpreted languages. By benchmarking basic operations, repeat addition, and file I/O, we evaluate how system overhead and language architecture impact execution time.

## 2. Methodology

Utilised two primary methods for measuring performance:

1. **External Timing:** Using the Linux `time` command to measure Real, User, and System time.
2. **Internal Timing:** Using `time.h` in C and `time` in Python to measure the duration of specific code blocks, excluding program startup and memory allocation overhead.

---

## 3. Benchmarking Data

The following data compares C and Python performance across file writing and reading operations with increasing scales (from 1,000 to 10,000,000 lines).

### File Write Performance

| Lines | Language | Internal Time (s) | Real (Wall) Time | User Time |
| --- | --- | --- | --- | --- |
| **1,000** | C | 0.000767 | 0.008s | 0.002s |
|  | Python | 0.002914 | 0.032s | 0.025s |
| **100,000** | C | 0.029183 | 0.084s | 0.009s |
|  | Python | 0.048824 | 0.085s | 0.049s |
| **10,000,000** | C | 0.923032 | 2.586s | 0.742s |
|  | Python | 4.398009 | 4.435s | 2.693s |

### File Read Performance

| Lines | Language | Internal Time (s) | Real (Wall) Time | User Time |
| --- | --- | --- | --- | --- |
| **1,000** | C | 0.000420 | 0.007s | 0.000s |
|  | Python | 0.001948 | 0.033s | 0.024s |
| **100,000** | C | 0.007799 | 0.027s | 0.005s |
|  | Python | 0.025836 | 0.056s | 0.032s |
| **10,000,000** | C | 0.786448 | 2.058s | 0.581s |
|  | Python | 2.183326 | 2.217s | 1.055s |

---

## 4. Evaluation & Conclusions

### C vs. Python Efficiency

Across all tests, C consistently outperformed Python. At the highest scale (10 million lines):

* **Writing:** C was approximately **4.7x faster** than Python in internal logic.
* **Reading:** C was approximately **2.7x faster** than Python.

This disparity is primarily due to Python being an interpreted language. The Python interpreter must translate code into machine instructions at runtime, whereas the C binary is already in machine code, allowing the CPU to execute instructions with minimal overhead.

### Scaling and Complexity

Both languages scaled linearly. As the input size increased by a factor of 10, the runtime generally increased by a factor of 10. However, the "startup cost" of Python is much more apparent at small scales (1,000 lines), where the Python `real` time is significantly higher than the internal time compared to C.

### System vs. User Time

In the C benchmarks, the `sys` time remained relatively low compared to `user` time until large-scale I/O was required. Interestingly, at 10 million lines, C showed a higher gap between `internal` and `real` time (0.9s vs 2.5s), likely due to the time required for the OS to flush the file buffers to the disk—a system-level task.

---

## 5. How to Run

To replicate these benchmarks use the following commands:

### C Programs
First create a data directory then add a file called output.txt
```bash
# Compile
gcc week_2/file_write.c -o bin/file_write
gcc week_2/file_read.c -o bin/file_read

# Run (Example with 1,000,000 lines)
time ./bin/file_write 1000000
time ./bin/file_read 1000000

```

### Python Programs

```bash
# Run (Example with 1,000,000 lines)
time python3 week_2/file_write.py 1000000
time python3 week_2/file_read.py 1000000

```

---

**Would you like me to double-check the logic in your `file_write.c` or `file_read.c` scripts to ensure they are handling the memory correctly for those 10-million-line tests?**
