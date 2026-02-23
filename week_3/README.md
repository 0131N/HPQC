

---

# Topic 4: MPI Communications – Lab Report

## 1. Introduction

This project explores the mechanics of message passing in distributed memory systems using MPI. Point-to-point communication modes are investigated, benchmark network latency and bandwidth via "Ping_Pong" tests. While comparing the preformance of manual communication loops against highly optimised collective operations.

---

## 2. Part 1: Demonstrating Point-to-Point Communications

###Initial Execution & Output Ordering (comm_test_mpi.c)
To establish a baseline, compiled and ran the un-functionalised comm_test_mpi code across varying numbers of processors (2, 3, 4, and 5).

Observations from the 4- and 5-Processor Runs:
When running with 4 and 5 processors, the terminal output became highly staggered. For example, in the 5-processor run:

Plaintext
Hello, I am 4 of 5. Sent 40 to Rank 0
Hello, I am 2 of 5. Sent 20 to Rank 0
Hello, I am 3 of 5. Sent 30 to Rank 0
Hello, I am 0 of 5. Received 10 from Rank 1
Hello, I am 0 of 5. Received 20 from Rank 2
Hello, I am 1 of 5. Sent 10 to Rank 0
Hello, I am 0 of 5. Received 30 from Rank 3
Hello, I am 0 of 5. Received 40 from Rank 4


Analysis of the Behavior:

Non-Deterministic Terminal Output: Worker 4 and Worker 2 print their "Sent" messages before Worker 1, and Rank 0's "Received" messages are mixed in between. This confirms that all worker processes are executing concurrently. Because each process runs in its own memory space, they hit their printf statements at slightly different times, and the operating system's standard output buffer flushes them to the terminal in a random, non-deterministic order.

Deterministic Receiving: Despite the chaotic order of the "Sent" print statements, Rank 0 always processes the received messages in perfect numerical order. This is because Rank 0 is executing a strict for loop, specificaly targeting MPI_Recv from Rank i. Even if Rank 4's message arrives at the destination node first, Rank 0 will leave it in the network buffer until it has finished receiving from Ranks 1, 2, and 3.

### Execution Order & System Noise

When running the baseline `MPI_Send` and `MPI_Recv` routines across multiple processors, the output logs appeared out of order. This is expected behavior. Because each process runs independently, network buffering and OS background tasks cause micro-fluctuations in execution time.

### MPI Send Modes Compared

Tested the four variants of the send command.

**Benchmarking Data (4-Processor Run)**

| Rank | `MPI_Ssend` Time (s) | `MPI_Bsend` Time (s) | `MPI_Isend` Time (s) |
| --- | --- | --- | --- |
| **Worker 1** | 0.000070 s | 0.000073 s | 0.000077 s |
| **Worker 2** | 0.000131 s | 0.000057 s | 0.000054 s |
| **Worker 3** | 0.000212 s | 0.000048 s | 0.000059 s |
| **Master (0)** | 0.000136 s | 0.000176 s | 0.000205 s |

*(Note: `MPI_Rsend` is excluded from the table as it caused the program to hang/crash in tests.)*

### Analysis of the Send Methods

1. **`MPI_Ssend` (Synchronous Send):** The safest, but potentially slowest method. The sender explicitly blocks until the receiver acknowledges it is ready to receive.
 There is a blocking penaltty in the data. Worker 3 took significantly longer than Worker 1 as the Master was busy talking to Worker 1 and Worker 2 first. That waiting time gets added to Worker 3's benchmark.


2. **`MPI_Rsend` (Ready Send):** Faster because ; skips the handshake entirely. If the receiver is not already waiting, the program will crash.
 In tests, running this caused the processes to hang indefinitely. Adding `MPI_Barrier` caused more failures here because the root process takes a microsecond longer to leave the barrier and reach its `MPI_Recv` than the workers take to reach `MPI_Rsend`.


3. **`MPI_Bsend` (Buffered Send):** Copies data into a buffer and returns immediately, decoupling the sender from the receiver.
 Because the workers just drop their data in a buffer and leave, their timings are incredibly fast and consistent , without the escalating delay seen in `Ssend`. However, it requires manual buffer allocation; failing to allocate enough memory causes a `MPI_ERR_BUFFER` crash.


4. **`MPI_Isend` (Non-Blocking Send):** Allows the process to initiate the send and immediately move on to other computations while the message files in the background.
 Similar to `Bsend`, the worker times are low and stable because they do not have to wait for the Master to be ready. This is highly efficient for overlapping calculation with communication.



---

## 3. Part 2: Benchmarking Latency and Bandwidth (Ping-Pong)

### Latency Convergence

Sending a message causes system overhead regardless of size. By sending an empty/tiny message back and forth, this latency was measured.

* **10 Pings:** Avg Latency = 0.000004 s (Highly susceptible to background CPU spikes).
* **10,000,000 Pings:** Avg Latency = 0.000000 s (Averages out noise, converging on stable microsecond latency).

### Bandwidth Calculation

Scaled the message size from 8 Bytes up to 2 MiB to measure network capacity.

* **8 Bytes:** 0.000000 s per message
* **1,024 Bytes (1 KiB):** 0.000001 s per message
* **1,048,576 Bytes (1 MiB):** 0.000127 s per message
* **2,097,152 Bytes (2 MiB):** 0.000248 s per message

**The Mathematical Model:**
These results map to a linear equation representing network transit:



Where:

*  = Total time to send
*  = Message size (Bytes)
*  = **Latency** (The y-intercept; time to send 0 bytes)
*  =  (The slope)

---

## 4. Part 3: Collective Communications

Benchmarked three different data distribution and collection methods across various array sizes and core counts.

### Data Distribution Architectures

* **Method 1: DIY (Send/Recv Loop):** The Master loops through workers, sending distinct chunks. The Master acts as a sequential bottleneck.
* **Method 2: Broadcast & Gather (`MPI_Bcast`):** The Master sends the *entire* array to everyone. Highly inefficient because it clogs network bandwidth with data the workers don't need.
* **Method 3: Scatter & Reduce (`MPI_Scatter` / `MPI_Reduce`):** The Master slices the array and deals distinct chunks. Operations are calculated simultaneously via an optimised network tree.

### 4-Core Benchmarking Results

| Array Size | Method 1 (DIY) | Method 2 (Bcast) | Method 3 (Scatter/Reduce) |
| --- | --- | --- | --- |
| **1,000** | 0.000040 s | 0.000060 s | **0.000011 s** |
| **100,000** | 0.000578 s | 0.001361 s | **0.000292 s** |
| **10,000,000** | 0.036731 s | 0.064964 s | **0.018434 s** |
| **1,000,000,000** | 5.300258 s | 8.425657 s | **2.646611 s** |

**Observation:** `MPI_Scatter` and `MPI_Reduce` (Method 3) consistently out-performed the other methods, processing 1 billion elements in half the time of the DIY method. As predicted, Broadcast was the slowest due to the massive memory overhead of duplicating 1 billion elements to every node.

### The Integer Truncation Anomaly (6 Cores)

During the 6-core test, the sum outputs returned anomalous values (e.g., `996` instead of `1000`, and `999999996` instead of `1,000,000,000`).

* **Cause:** This is a result of truncation. When slicing an array of 1,000 elements across 6 processors, the C integers leave 4 elements uncalculated. In production environments, remainder logic must be implemented to handle array sizes not perfectly divisible by the processor count.

---

## 5. Custom Reduce Operation

used `MPI_Op_create()` to build a custom array summation function and compared it against the built-in `MPI_SUM` operator.

### Accuracy & Performance (4 Cores)

| Array Size | Built-in Sum | Custom Sum | Built-in Time | Custom Time |
| --- | --- | --- | --- | --- |
| **1,000** | 1,000 | 1,000 | 0.000010 s | 0.000005 s |
| **100,000** | 100,000 | 100,000 | 0.000018 s | 0.000003 s |
| **10,000,000** | 10,000,000 | 10,000,000 | 0.000014 s | 0.000001 s |
| **1,000,000,000** | 1,000,000,000 | 1,000,000,000 | 0.000030 s | 0.000001 s |

**Conclusion:** The custom operation maintained 100% mathematical accuracy compared to the standard `MPI_SUM`. Interestingly, the custom implementation reported consistently lower timing overheads (often registering at just 1 microsecond). This extreme speed in the custom op timings is likely due to CPU cache hits on localised data or timing placement relative to the operator initialisation, though the built-in `MPI_SUM` remains the standard for portability and hardware-level network optimisations.

---
