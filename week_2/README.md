Task 1
Compiled code for Hello world code named Hello.c
Ran code for 4
Hello world from rank 0 out of 4 processors
Hello world from rank 2 out of 4 processors
Hello world from rank 3 out of 4 processors
Hello world from rank 1 out of 4 processors
 from rank 3 out of 4 processors
Hello world from rank 1 out of 4 processors

3

3
Hello world from rank 1 out of 3 processors
Hello world from rank 2 out of 3 processors
Hello world from rank 0 out of 3 processors

real    0m0.410s
user    0m0.100s
sys     0m0.143s

4
Hello world from rank 1 out of 4 processors
Hello world from rank 2 out of 4 processors
Hello world from rank 0 out of 4 processors
Hello world from rank 3 out of 4 processors

real    0m0.409s
user    0m0.120s
sys     0m0.162s

5
Hello world from rank 3 out of 5 processors
Hello world from rank 4 out of 5 processors
Hello world from rank 0 out of 5 processors
Hello world from rank 1 out of 5 processors
Hello world from rank 2 out of 5 processors

real    0m0.418s
user    0m0.160s
sys     0m0.170s

6
Hello world from rank 4 out of 6 processors
Hello world from rank 0 out of 6 processors
Hello world from rank 1 out of 6 processors
Hello world from rank 2 out of 6 processors
Hello world from rank 5 out of 6 processors
Hello world from rank 3 out of 6 processors

real    0m0.423s
user    0m0.196s
sys     0m0.189s


Task 2
main()Setup:
 Initializes the MPI environment (sets up the communication lines) and retrieves the "ID card" (rank) for the current process and the total count of processes (universe size)
.Validation: Calls check_args immediately to ensure the user provided the necessary input number. It also calls check_uni_size to ensure the MPI universe is large enough to run.
Execution: Calls check_task, which acts as a traffic director, sending the process to do either "root work" or "client work" depending on its rank
.Cleanup: Shuts down the MPI environment safely before the program exits.check_args(argc, argv)Input Check: Looks at the command line inputs. It requires exactly 2 items: the program name and one number
.Conversion: If the input is valid, it converts the text string (e.g., "10") into an actual integer number.Error Handling: If the argument is missing, it prints a helpful error message explaining how to use the program and then forces the program to quit immediately.check_uni_size(uni_size)Safety Check: Verifies that the "universe" (number of processes) is at least 1.
Error Handling: If for some reason the universe is too small (less than 1), it prints an error and quits. Note: In this specific code, since min_uni_size is set to 1, this acts as a basic sanity check.check_task(uni_size, my_rank, num_arg
)The Decision Maker: This function splits the universe into two groups:Rank 0: is assigned the root_task.Everyone else (Rank > 0): is assigned the client_task.client_task(my_rank, num_arg)Calculation: Multiplies its own rank ID by the user-provided number (num_arg).Example: If Rank is 3 and input is 10, it calculates 30.
Transmission: Sends this single calculated value specifically to Rank 0.root_task(uni_size)Preparation: Sets up a running total variable (output_sum) starting at 0
Collection Loop: Loops through every other rank in the universe (from 1 up to uni_size - 1).Wait & Receive: Stops and waits for a message from the specific rank it is currently looping on.Accumulate: Takes the value received and adds it to the output_sum.Result: Prints the final total to the screen.2.
 The Mathematical FormulaYou asked for a one-operation mathematical formula that achieves the same result.Let's look at what the code effectively does:Input:
 Let $A$ be the argument provided by the user.Process: Every process $i$ (where $i > 0$) calculates $i \times A$.Summation:
 The root sums these up from rank $1$ to rank $N-1$ (where $N$ is the universe size).Mathematically,
 this is the sum:$$\text{Total} = \sum_{i=1}^{N-1} (i \times A)$$We can factor out $A$:$$\text{Total} = A \times \sum_{i=1}^{N-1} i$$The sum of integers from $1$ to $k$ is a known arithmetic series formula: $\frac{k(k+1)}{2}$.In our case,
 $k = N-1$. Substituting this in:$$\text{Total} = A \times \frac{(N-1) \times ((N-1)+1)}{2}$$Simplifying gives us the final formula:$$\text{Total} = A \times \frac{N(N-1)}{2}$$Verification:Using your example of 4 processors ($N=4$) and input 10 ($A=10$)
:Code: $10 + 20 + 30 = 60$Formula: $10 \times \frac{4 \times 3}{2} = 10 \times \frac{12}{2} = 10 \times 6 = 60$


Task 3


main()Initialization: The program begins by setting up the environment. It immediately calls check_args to parse the command-line input and determine the size of the vector ($N$).
Memory Allocation: It dynamically requests a contiguous block of memory from the system (using malloc) large enough to hold $N$ integers. This creates the array my_vector.
Data Preparation: It calls initialise_vector to fill this newly created array with a starting value (in the original code, this is 0; in your modification, it might be 1).
Computation: It calls sum_vector, passing the filled array and its size. This function performs the actual mathematical work and returns the total.Output: The result (my_sum) is printed to the terminal.
Cleanup: The memory block my_vector is released back to the system (free) to prevent memory leaks before the program exits.check_args(argc, argv)
Input Validation: Checks if the user provided exactly one argument after the program name (argc == 2).
Conversion: Parses the string argument (e.g., "1000") into an integer variable (num_arg).
Error Handling: If the argument count is wrong, it prints usage instructions to stderr and terminates the program immediately with an exit code of -1.
initialise_vector(vector, size, initial)
Traversal: Sets up a loop that runs from index $0$ to $size - 1$.
Assignment: In each iteration $i$, it accesses the memory address at vector[i] and writes the value initial into it.
Result: After the loop finishes, the array contains $N$ identical integers.sum_vector(vector, size)Accumulator Setup: Declares a local integer variable sum and initializes it to 0.
Traversal: Sets up a loop that iterates through the vector from index $0$ to $size - 1$.Accumulation: In each iteration, it reads the value at vector[i] and adds it to the running total sum.
Return: Once the loop is complete, it returns the final value of sum to the caller.print_vector(vector, size)Traversal: Iterates through the vector from index $0$ to $size - 1$.Output: Prints the value of each element vector[i] to the screen on a new line. (Note: This function is defined but often commented out or n.

for 10
real    0m0.007s
user    0m0.000s
sys     0m0.004s

For 1,000,000
real    0m0.017s
user    0m0.010s
sys     0m0.005s
For 10,000,000
real    0m0.081s
user    0m0.064s
sys     0m0.016s

For 100,000,000
real    0m0.703s
user    0m0.529s
sys     0m0.173s

For 1,000,000,000
real    0m8.328s
user    0m5.977s
sys     0m2.000s

Added parallel processing 
With 4 cores
for 10
real    0m0.417s
user    0m0.145s
sys     0m0.144s
for 1,000,000
real    0m0.421s
user    0m0.116s
sys     0m0.181s
for 10,000,000
real    0m0.437s
user    0m0.209s
sys     0m0.168s
for 100,000,000
real    0m0.603s
user    0m0.696s
sys     0m0.327s
for 1,000,000,000
real    0m2.458s
user    0m6.135s
sys     0m2.294s
with 8 cores
for 10
real    0m0.428s
user    0m0.254s
sys     0m0.224s
for 1,000,000
real    0m0.429s
user    0m0.221s
sys     0m0.271s
for 10,000,000
real    0m0.446s
user    0m0.287s
sys     0m0.307s
for 100,000,000
real    0m0.541s
user    0m0.843s
sys     0m0.487s
for 1,000,000,000
real    0m1.426s
user    0m6.195s
sys     0m2.198s



