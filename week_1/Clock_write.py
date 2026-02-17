import time
import sys

if len(sys.argv) != 2:
    print("Usage: python Clock_write.py <number_of_lines>")
    sys.exit(1)

num_lines = int(sys.argv[1])

start_time = time.time()

with open("data/test_output.txt", "w") as f:
    for i in range(num_lines):
        f.write(f"Line number {i}\n")

end_time = time.time()
print(f"Time taken to write {num_lines} lines: {end_time - start_time} seconds")
