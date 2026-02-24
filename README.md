
---

# String Oscillation Simulation: From Serial Logic to MPI Physics



## Part 1: Architecture & Flexibility

The initial serial code was restructured to handle dynamic user input. I implemented a `SimConfig` struct to bundle simulation parameters, allowing for easier data management and cleaner function signatures.

* **Command Line Arguments:** Users can specify `points`, `cycles`, `samples`, and `filepath` at runtime.
* **Memory Management:** Arrays are dynamically allocated based on the `points` argument, ensuring the simulation scales from small test cases to high-resolution models.

---

## Part 2: Parallel Strategy & Aggregation

### Domain Decomposition

To parallelise the workload, the string is divided into equal chunks. Each MPI rank is responsible for its local segment. However, since wave propagation is interdependent, I implemented Ghost Cell Exchange.

* **Left-to-Right Propagation:** Rank  sends its last point to Rank  and receives the last point of Rank  as a ghost cell.
* **Aggregation Strategy:** To avoid race conditions and file corruption, an In-Memory Aggregation strategy was used.
1. Worker ranks compute their local physics.
2. `MPI_Gather` collects all local arrays onto Rank 0.
3. **Rank 0** performs the serial I/O task of writing the full string state to the CSV.



---

##  Part 3: Hooke's Law & Cartesian Topology

### Improved Physics Model

The simplistic model was replaced with a Newtonian spring-mass system. Each point  is treated as a mass  connected by springs of stiffness . The acceleration  is calculated using the relative displacement of neighbors:

The simulation tracks both Position and Velocity, updating them via the Semi-Implicit Euler method:

1. 
2. 

### Cartesian Communicator

To handle the bidirectional communication required by Hooke's Law , I implemented a 1D Cartesian Topology using `MPI_Cart_create`. This allows for neighbor discovery via `MPI_Cart_shift`, which automatically handles the boundaries .


---

## Benchmarking

### Serial vs. Parallel

Wall-clock times:

| Points () | 1 Processor (Serial) | 2 Processors | 5 Processors |
| --- | --- | --- | --- |
| 5,000 | 0.129s | 0.127s | 0.173s |
| 50,000 | 1.249s | 1.242s | 1.339s |
| 500,000 | **12.500s** | **12.438s** | **14.088s** |

**Analysis of Bottlenecks:**
The data reveals an Overhead. In theory, 5 processors should be nearly 5x faster than one. However, the 5-processor run was actually 12.7% slower than the serial run at 500,000 points.

1. **The I/O Bottleneck:** The current code gathers all data to Rank 0 to write a CSV. Writing 500,000 columns per row to a disk is a massive serial operation.
2. **Aggregation Costs:** `MPI_Gather` requires moving 500,000 doubles across the network/interconnect. The time spent communicating outweighs the time saved in the simple  calculation loop.

 Impact of k/m on Computational Load

In Part 3, I varied the stiffness-to-mass ratio (k/m). While k/m is a physical constant, it has a profound impact on the stability of the numerical solver.

|  Value | Time (4 Processors, 1000 Pts) | State |
| --- | --- | --- |
| 10 | 0.123s | Stable |
| 800 | 0.176s | Stable |
| 3200 | 0.574s | Approaching Instability |
| 6400 | **2.770s** | **Numerical Explosion** |

**time increase**
When  is too high, the acceleration values exceed the precision of the `double` data type within the given time step . This leads to `NaN` or `Inf` values. Processing these "non-numbers" in the floating-point unit and writing them as long text strings to the CSV significantly slows down the execution time. This is a violation of the Courant-Friedrichs-Lewy Condition, which dictates that information cannot travel across the simulation grid faster than the time-step allows.

---

## Viability of Parallelism

**Does this task demand a parallel solution?**

* **For the current implementation: No.** Because the calculation for Hooke's Law is mathematically non-taxing and the I/O  is computationally expensive, adding more cores only increases the communication overhead without solving the primary bottleneck .
* **When does it become viable?** Parallelisation would become viable if:
1. **I used Parallel I/O:** Using `MPI-IO` to let every rank write to different parts of the file simultaneously.
2. **Increased Complexity:** If the problem moved to a 3D wave equation or a complex Molecular Dynamics simulation where the math per point is much more intense.
3. **Higher Resolution:** At millions of points, the memory might exceed a single machine's RAM, making distributed memory  a requirement for capacity, even if not for speed.



---

## Graph Analysis

### Part 2: The "Copy" Model

The animation for Part 2 shows a perfect, non-dispersive sine wave sliding to the right.

* **Does it make sense?** Yes. Since each point  simply takes the value of  from the previous step, the initial driver signal is shifted perfectly. There is no tension to pull the string back, so the wave never changes shape or reflects; it simply exits the domain.

### The Hooke's Law Model

The animation for Part 3 shows a wave that oscillates, dampens slightly, and most reflects.

* **Does it make sense?**  Yes. By using , I allow the string to act as a continuous medium. When the wave hits the fixed boundary (`local_pos[last] = 0`), the stored potential energy in the spring at the wall exerts a counter-force, sending the wave back in the opposite direction. This mimics a real system.

---





---

## How to Run

### Compilation

```bash
mpicc HPQC/week_4/string_wave_hookes_law.c -o bin/string_wave_hookes_law -lm

```

### Execution

```bash
mpirun -np 4 ./bin/string_wave_hookes_law <points> <cycles> <samples> <output.csv>

```

### Visualisation

```bash
python3 animate_line_file_updated.py data/output.csv my_animation

```
