import time

start_time = time.time()

try:
    with open("data/test_output.txt", "r") as f:
        for line in f:
            pass

    end_time = time.time()
    print(f"Time taken to read: {end_time - start_time} seconds")

except FileNotFoundError:
    print("Error: data/test_output.txt not found. Run the write program first!")
